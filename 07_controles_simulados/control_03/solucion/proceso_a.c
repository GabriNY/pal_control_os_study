/*
 * CONTROL 03 - El Pipe de C se redirige a STDIN usando dup2().
 * SIGINT  -> A -> MQ -> B
 * SIGUSR1 -> A -> Pipe -> dup2 -> STDIN de C
 */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <unistd.h>

#define MQ_KEY 0x6103

struct mensaje {
    long type;
    char text[128];
};

static volatile sig_atomic_t sigint_flag = 0;
static volatile sig_atomic_t usr1_flag = 0;

static void handler(int signo)
{
    if (signo == SIGINT) sigint_flag = 1;
    if (signo == SIGUSR1) usr1_flag = 1;
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
        close(fd[1]);

        pid_t clon = fork();
        if (clon == -1) _exit(1);

        if (clon == 0) {
            /* Duplica fd[0] sobre descriptor 0. C podrá usar read(STDIN_FILENO). */
            if (dup2(fd[0], STDIN_FILENO) == -1) {
                perror("dup2");
                _exit(1);
            }
            close(fd[0]); /* La copia original ya no es necesaria. */

            char *args[] = {"./proceso_c", NULL};
            execv(args[0], args);
            perror("execv C");
            _exit(1);
        }

        /* B no usa Pipe: su canal es exclusivamente Message Queue. */
        close(fd[0]);
        struct mensaje msg;
        if (msgrcv(qid, &msg, sizeof(msg.text), 3, 0) == -1) _exit(1);
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
    sigaction(SIGUSR1, &sa, NULL);

    printf("[A PID=%d]\n", getpid());
    printf("  kill -INT  %d -> MQ -> B\n", getpid());
    printf("  kill -USR1 %d -> Pipe -> STDIN -> C\n", getpid());

    int b_ok = 0;
    int c_ok = 0;
    while (!b_ok || !c_ok) {
        pause();

        if (sigint_flag && !b_ok) {
            sigint_flag = 0;
            struct mensaje msg = {.type = 3};
            strcpy(msg.text, "Evento SIGINT para B");
            msgsnd(qid, &msg, strlen(msg.text) + 1, 0);
            b_ok = 1;
        }

        if (usr1_flag && !c_ok) {
            usr1_flag = 0;
            const char texto[] = "Evento SIGUSR1 recibido por C mediante STDIN\n";
            write(fd[1], texto, sizeof(texto) - 1);
            c_ok = 1;
        }
    }

    close(fd[1]);
    waitpid(pid_b, NULL, 0);
    msgctl(qid, IPC_RMID, NULL);
    return 0;
}
