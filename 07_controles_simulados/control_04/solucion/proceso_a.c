/*
 * CONTROL 04 - SOLUCIÓN PRINCIPAL CORRECTA.
 * Se deja en un solo archivo A+B y un ejecutable proceso_c separado.
 */
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define MQ_KEY 0x6104
#define FIFO_PATH "/tmp/control04_fifo"

struct mensaje {
    long type;
    char text[200];
};

static volatile sig_atomic_t usr1 = 0;
static volatile sig_atomic_t usr2 = 0;

static void handler(int signo)
{
    if (signo == SIGUSR1) usr1 = 1;
    if (signo == SIGUSR2) usr2 = 1;
}

static int enviar_mq(int qid, long type, const char *texto)
{
    struct mensaje msg = {.type = type};
    snprintf(msg.text, sizeof(msg.text), "%s", texto);
    return msgsnd(qid, &msg, strlen(msg.text) + 1, 0);
}

int main(void)
{
    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return 1;
    }

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
    if (pid_b == -1) return 1;

    if (pid_b == 0) {
        /* IMPORTANTE: B crea C antes de cerrar el extremo de lectura. */
        pid_t clon = fork();
        if (clon == -1) _exit(1);

        if (clon == 0) {
            close(fd[1]);
            if (dup2(fd[0], STDIN_FILENO) == -1) _exit(1);
            close(fd[0]);
            char *args[] = {"./proceso_c", NULL};
            execv(args[0], args);
            perror("execv C");
            _exit(1);
        }

        /* B original es escritor del Pipe y receptor de Message Queue. */
        close(fd[0]);

        for (int i = 0; i < 2; ++i) {
            struct mensaje msg;
            if (msgrcv(qid, &msg, sizeof(msg.text), 0, 0) == -1) {
                perror("B msgrcv");
                _exit(1);
            }

            printf("[B PID=%d] MQ type=%ld: %s\n", getpid(), msg.type, msg.text);
        fflush(stdout);

            /* Agregamos salto de línea para que C pueda usar fgets() como framing. */
            char linea[256];
            int len = snprintf(linea, sizeof(linea), "type=%ld | %s\n", msg.type, msg.text);
            write(fd[1], linea, (size_t)len);
        }

        close(fd[1]);
        waitpid(clon, NULL, 0);
        _exit(0);
    }

    /* A no utiliza el Pipe directamente en este control. */
    close(fd[0]);
    close(fd[1]);

    struct sigaction sa = {0};
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("[A PID=%d]\n", getpid());
    printf("Evento 1: kill -USR1 %d\n", getpid());
    printf("          luego: echo 'dato desde FIFO' > %s\n", FIFO_PATH);
    printf("Evento 2: kill -USR2 %d\n", getpid());

    int enviado_1 = 0;
    int enviado_2 = 0;

    while (!enviado_1 || !enviado_2) {
        pause();

        if (usr1 && !enviado_1) {
            usr1 = 0;

            /* A se bloquea aquí hasta que otra terminal abra el FIFO para escribir. */
            int fifo = open(FIFO_PATH, O_RDONLY);
            if (fifo == -1) {
                perror("open FIFO");
                continue;
            }

            char buffer[180];
            ssize_t n = read(fifo, buffer, sizeof(buffer) - 1);
            close(fifo);

            if (n > 0) {
                buffer[n] = '\0';
                buffer[strcspn(buffer, "\n")] = '\0';
                if (enviar_mq(qid, 1, buffer) == -1) perror("msgsnd type1");
                else enviado_1 = 1;
            }
        }

        if (usr2 && !enviado_2) {
            usr2 = 0;
            if (enviar_mq(qid, 2, "estado generado internamente por A") == -1)
                perror("msgsnd type2");
            else
                enviado_2 = 1;
        }
    }

    waitpid(pid_b, NULL, 0);
    msgctl(qid, IPC_RMID, NULL);
    unlink(FIFO_PATH);
    return 0;
}
