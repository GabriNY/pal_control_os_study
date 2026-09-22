/* CONTROL 05 - C confirma la recepción enviando SIGUSR1 al PID de A. */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Uso: %s fd_pipe pid_A\n", argv[0]);
        return 1;
    }

    int fd = atoi(argv[1]);
    pid_t pid_a = (pid_t)atoi(argv[2]);
    char buffer[256];

    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';
        printf("[C PID=%d] Pipe: %s", getpid(), buffer);

        /* kill() también puede ser usado por procesos para enviarse señales entre sí. */
        if (kill(pid_a, SIGUSR1) == -1) {
            perror("kill ACK");
        }
    }

    close(fd);
    return 0;
}
