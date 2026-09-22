/*
 * CONTROL 06 - Herencia del descriptor a través de execv().
 * Se comprueba FD_CLOEXEC antes y después del execv.
 */
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

#define MQ_KEY 0x6106

struct mensaje {
    long type;
    char text[128];
};

static volatile sig_atomic_t sigint_flag = 0;
static volatile sig_atomic_t sigtrap_flag = 0;

static void handler(int signo)
{
    if (signo == SIGINT) sigint_flag = 1;
    if (signo == SIGTRAP) sigtrap_flag = 1;
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
    if (pid_b == -1) return 1;

    if (pid_b == 0) {
        close(fd[1]);

        pid_t clon = fork();
        if (clon == -1) _exit(1);

        if (clon == 0) {
            /* F_GETFD consulta las banderas asociadas al descriptor. */
            int flags = fcntl(fd[0], F_GETFD);
            if (flags == -1) {
                perror("fcntl F_GETFD");
                _exit(1);
            }

            printf("[Clon B] fd=%d antes de execv: FD_CLOEXEC %s\n",
                   fd[0], (flags & FD_CLOEXEC) ? "ACTIVADO" : "DESACTIVADO");
            fflush(stdout);

            char descriptor[32];
            snprintf(descriptor, sizeof(descriptor), "%d", fd[0]);
            char *args[] = {"./proceso_c", descriptor, NULL};
            execv(args[0], args);
            perror("execv C");
            _exit(1);
        }

        close(fd[0]);

        struct mensaje msg;
        if (msgrcv(qid, &msg, sizeof(msg.text), 7, 0) == -1) {
            perror("B msgrcv");
            _exit(1);
        }
        printf("[B PID=%d] MQ: %s\n", getpid(), msg.text);
        fflush(stdout);

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

    printf("[A PID=%d]\n", getpid());
    printf("  kill -INT  %d -> MQ type=7 -> B\n", getpid());
    printf("  kill -TRAP %d -> Pipe -> C\n", getpid());

    int b_ok = 0;
    int c_ok = 0;
    while (!b_ok || !c_ok) {
        pause();

        if (sigint_flag && !b_ok) {
            sigint_flag = 0;
            struct mensaje msg = {.type = 7};
            strcpy(msg.text, "Mensaje para B en control FD_CLOEXEC");
            msgsnd(qid, &msg, strlen(msg.text) + 1, 0);
            b_ok = 1;
        }

        if (sigtrap_flag && !c_ok) {
            sigtrap_flag = 0;
            const char texto[] = "El descriptor sobrevivió al execv si C puede leer esto\n";
            write(fd[1], texto, sizeof(texto) - 1);
            c_ok = 1;
        }
    }

    close(fd[1]);
    waitpid(pid_b, NULL, 0);
    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
