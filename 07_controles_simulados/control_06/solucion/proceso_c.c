/* CONTROL 06 - Verifica el descriptor heredado después de execv(). */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Uso: %s descriptor\n", argv[0]);
        return 1;
    }

    int fd = atoi(argv[1]);

    /* Si execv hubiera cerrado el descriptor, F_GETFD retornaría -1/EBADF. */
    int flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        perror("C fcntl: descriptor no sobrevivió");
        return 1;
    }

    printf("[C PID=%d] fd=%d después de execv: FD_CLOEXEC %s\n",
           getpid(), fd, (flags & FD_CLOEXEC) ? "ACTIVADO" : "DESACTIVADO");

    char buffer[256];
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        buffer[n] = '\0';
        printf("[C] recibido: %s", buffer);
    }

    close(fd);
    return 0;
}
