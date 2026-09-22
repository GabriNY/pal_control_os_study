/* CONTROL 01 - C recibe por argv el descriptor de lectura del Pipe. */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Uso: %s descriptor_pipe\n", argv[0]);
        return 1;
    }

    /* El descriptor llega como texto porque execv solo transporta argv strings. */
    int fd = atoi(argv[1]);
    char buffer[256];

    /* El descriptor sigue abierto porque fue heredado y no tiene FD_CLOEXEC. */
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    if (n == -1) {
        perror("C read");
        return 1;
    }

    if (n > 0) {
        buffer[n] = '\0';
        printf("[C PID=%d] Pipe fd=%d: %s", getpid(), fd, buffer);
    }

    close(fd);
    return 0;
}
