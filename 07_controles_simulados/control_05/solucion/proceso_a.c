/*
 * CONTROL 05 - Confirmaciones de recepción.
 * SIGINT  -> A -> MQ(type=1) -> B -> MQ(type=2 ACK) -> A
 * SIGTRAP -> A -> Pipe -> C -> kill(SIGUSR1) -> A
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

#define MQ_KEY 0x6105

struct mensaje {
    long type;
    char text[160];
};

static volatile sig_atomic_t cli_int = 0;
static volatile sig_atomic_t cli_trap = 0;
static volatile sig_atomic_t ack_c = 0;

static void handler(int signo)
{
    if (signo == SIGINT) cli_int = 1;
    if (signo == SIGTRAP) cli_trap = 1;
    if (signo == SIGUSR1) ack_c = 1;
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

    /* Guardamos PID_A antes de fork: B lo hereda y podrá pasarlo a C. */
    pid_t pid_a = getpid();

    pid_t pid_b = fork();
    if (pid_b == -1) return 1;

    if (pid_b == 0) {
        close(fd[1]);

        pid_t clon = fork();
        if (clon == -1) _exit(1);

        if (clon == 0) {
            char fd_texto[32];
            char pid_a_texto[32];
            snprintf(fd_texto, sizeof(fd_texto), "%d", fd[0]);
            snprintf(pid_a_texto, sizeof(pid_a_texto), "%d", (int)pid_a);

            char *args[] = {"./proceso_c", fd_texto, pid_a_texto, NULL};
            execv(args[0], args);
            perror("execv C");
            _exit(1);
        }

        /* B no usa Pipe; deja ese descriptor únicamente en C. */
        close(fd[0]);

        struct mensaje recibido;
        if (msgrcv(qid, &recibido, sizeof(recibido.text), 1, 0) == -1) {
            perror("B msgrcv");
            _exit(1);
        }

        printf("[B PID=%d] recibido: %s\n", getpid(), recibido.text);
        fflush(stdout);

        /* ACK de B para A mediante la MISMA cola, pero otro tipo. */
        struct mensaje ack = {.type = 2};
        strcpy(ack.text, "ACK de B: Message Queue recibida correctamente");
        msgsnd(qid, &ack, strlen(ack.text) + 1, 0);

        waitpid(clon, NULL, 0);
        _exit(0);
    }

    close(fd[0]);

    struct sigaction sa = {0};
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTRAP, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    printf("[A PID=%d]\n", getpid());
    printf("  kill -INT  %d -> B; B responde ACK por MQ\n", getpid());
    printf("  kill -TRAP %d -> C; C responde SIGUSR1\n", getpid());

    int flujo_b = 0;
    int flujo_c = 0;

    while (!flujo_b || !flujo_c || !ack_c) {
        pause();

        if (cli_int && !flujo_b) {
            cli_int = 0;
            struct mensaje msg = {.type = 1};
            strcpy(msg.text, "Solicitud de A para B");
            msgsnd(qid, &msg, strlen(msg.text) + 1, 0);

            /* A espera explícitamente el ACK type=2 de B. */
            struct mensaje ack;
            if (msgrcv(qid, &ack, sizeof(ack.text), 2, 0) != -1) {
                printf("[A] %s\n", ack.text);
                flujo_b = 1;
            }
        }

        if (cli_trap && !flujo_c) {
            cli_trap = 0;
            const char texto[] = "Solicitud de A para C por Pipe\n";
            write(fd[1], texto, sizeof(texto) - 1);
            flujo_c = 1;
        }

        if (ack_c) {
            printf("[A] ACK de C recibido como SIGUSR1\n");
        }
    }

    close(fd[1]);
    waitpid(pid_b, NULL, 0);
    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
