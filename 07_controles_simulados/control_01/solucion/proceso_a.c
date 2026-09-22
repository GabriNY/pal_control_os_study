/*
 * CONTROL 01 - proceso_a.c
 * Flujo 1: SIGINT  -> A -> Message Queue -> B
 * Flujo 2: SIGTRAP -> A -> Pipe          -> C
 * Jerarquía: A -> fork(B) -> fork(Clon B) -> execv(C)
 */
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MQ_KEY 0x6101

struct mensaje {
    long type;              /* System V exige que el tipo sea long y vaya primero. */
    char text[128];         /* Cuerpo real que viajará por la cola. */
};

/* El handler no hace IPC: solo avisa al main qué señal llegó. */
static volatile sig_atomic_t evento_sigint = 0;
static volatile sig_atomic_t evento_sigtrap = 0;

static void manejar_senal(int signo)
{
    if (signo == SIGINT) {
        evento_sigint = 1;
    } else if (signo == SIGTRAP) {
        evento_sigtrap = 1;
    }
}

int main(void)
{
    int fd[2];

    /* El Pipe se crea ANTES de fork para que B y el futuro C hereden fd[0]. */
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    /* La cola también existe antes de fork; A y B usarán el mismo qid. */
    int qid = msgget(MQ_KEY, IPC_CREAT | 0666);
    if (qid == -1) {
        perror("msgget");
        return 1;
    }

    pid_t pid_b = fork();
    if (pid_b == -1) {
        perror("fork B");
        return 1;
    }

    if (pid_b == 0) {
        /* ======================= PROCESO B ======================= */
        close(fd[1]); /* B no escribe al Pipe. */

        /* B crea el clon que posteriormente se convertirá en C. */
        pid_t clon = fork();
        if (clon == -1) {
            perror("fork clon B");
            _exit(1);
        }

        if (clon == 0) {
            /* ================ CLON DE B -> PROCESO C ================ */
            char descriptor[32];

            /* execv recibe strings: convertimos fd[0] de entero a texto. */
            snprintf(descriptor, sizeof(descriptor), "%d", fd[0]);

            char *args[] = {"./proceso_c", descriptor, NULL};
            execv(args[0], args);

            /* Solo se llega aquí si execv falló. */
            perror("execv proceso_c");
            _exit(1);
        }

        /* B original ya no leerá el Pipe; solamente espera Message Queue. */
        close(fd[0]);

        struct mensaje recibido;
        if (msgrcv(qid, &recibido, sizeof(recibido.text), 2, 0) == -1) {
            perror("B msgrcv");
            _exit(1);
        }

        printf("[B PID=%d] MQ type=%ld: %s\n",
               getpid(), recibido.type, recibido.text);
        fflush(stdout);

        /* B es padre de C, por eso B debe recogerlo con waitpid(). */
        waitpid(clon, NULL, 0);
        _exit(0);
    }

    /* ======================= PROCESO A ======================= */
    close(fd[0]); /* A solo escribirá en el Pipe hacia C. */

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = manejar_senal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTRAP, &sa, NULL);

    printf("[A PID=%d]\n", getpid());
    printf("  kill -INT  %d  -> Message Queue hacia B\n", getpid());
    printf("  kill -TRAP %d  -> Pipe hacia C\n", getpid());

    int enviado_b = 0;
    int enviado_c = 0;

    while (!enviado_b || !enviado_c) {
        pause(); /* A permanece en espera pasiva de señales. */

        if (evento_sigint && !enviado_b) {
            evento_sigint = 0;

            struct mensaje msg = {.type = 2};
            snprintf(msg.text, sizeof(msg.text),
                     "Mensaje generado por A después de SIGINT");

            if (msgsnd(qid, &msg, strlen(msg.text) + 1, 0) == -1) {
                perror("A msgsnd");
            } else {
                printf("[A] SIGINT -> MQ -> B\n");
                enviado_b = 1;
            }
        }

        if (evento_sigtrap && !enviado_c) {
            evento_sigtrap = 0;
            const char texto[] = "Mensaje generado por A después de SIGTRAP\n";

            if (write(fd[1], texto, sizeof(texto) - 1) == -1) {
                perror("A write pipe");
            } else {
                printf("[A] SIGTRAP -> Pipe -> C\n");
                enviado_c = 1;
            }
        }
    }

    close(fd[1]);             /* EOF para C si todavía estuviera leyendo. */
    waitpid(pid_b, NULL, 0);  /* A recoge a B. */
    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
