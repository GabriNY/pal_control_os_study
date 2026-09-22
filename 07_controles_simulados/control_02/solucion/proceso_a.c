/*
 * CONTROL 02 - Canales invertidos.
 * SIGUSR1 -> A -> Pipe -> B
 * SIGUSR2 -> A -> Message Queue -> C
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

#define MQ_KEY 0x6102

struct mensaje {
    long type;
    char text[128];
};

static volatile sig_atomic_t usr1 = 0;
static volatile sig_atomic_t usr2 = 0;

static void handler(int signo)
{
    if (signo == SIGUSR1) usr1 = 1;
    if (signo == SIGUSR2) usr2 = 1;
}

int main(void)
{
    int fd[2];
    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

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
        /* B será lector del Pipe. */
        close(fd[1]);

        pid_t clon = fork();
        if (clon == -1) {
            perror("fork clon");
            _exit(1);
        }

        if (clon == 0) {
            /* C no necesita el Pipe: cierra la copia heredada. */
            close(fd[0]);

            /* Pasamos qid como argumento para que C conozca la cola exacta. */
            char qid_texto[32];
            snprintf(qid_texto, sizeof(qid_texto), "%d", qid);
            char *args[] = {"./proceso_c", qid_texto, NULL};
            execv(args[0], args);
            perror("execv C");
            _exit(1);
        }

        char buffer[256];
        ssize_t n = read(fd[0], buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("[B PID=%d] recibido por Pipe: %s", getpid(), buffer);
        fflush(stdout);
        }
        close(fd[0]);
        waitpid(clon, NULL, 0);
        _exit(0);
    }

    /* A solo escribe al Pipe. */
    close(fd[0]);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("[A PID=%d]\n", getpid());
    printf("  kill -USR1 %d -> Pipe -> B\n", getpid());
    printf("  kill -USR2 %d -> MQ type=5 -> C\n", getpid());

    int listo_b = 0;
    int listo_c = 0;
    while (!listo_b || !listo_c) {
        pause();

        if (usr1 && !listo_b) {
            usr1 = 0;
            const char texto[] = "A envia por Pipe al proceso B\n";
            write(fd[1], texto, sizeof(texto) - 1);
            listo_b = 1;
        }

        if (usr2 && !listo_c) {
            usr2 = 0;
            struct mensaje msg = {.type = 5};
            strcpy(msg.text, "A envia por Message Queue al proceso C");
            msgsnd(qid, &msg, strlen(msg.text) + 1, 0);
            listo_c = 1;
        }
    }

    close(fd[1]);
    waitpid(pid_b, NULL, 0);
    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
