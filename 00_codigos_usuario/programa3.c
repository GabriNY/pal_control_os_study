/*
 * PROGRAMA 3 - Recibe por argv el número de un descriptor heredado.
 * Además comprueba FD_CLOEXEC para explicar por qué el Pipe sobrevivió al execv().
 */
#include <fcntl.h>   /* fcntl(), F_GETFD, FD_CLOEXEC */
#include <stdio.h>
#include <stdlib.h>  /* atoi() */
#include <unistd.h>  /* read(), close() */

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Uso: %s descriptor\n", argv[0]);
        return 1;
    }

    /* argv[1] contiene, por ejemplo, "3". atoi lo convierte otra vez a int. */
    int fd = atoi(argv[1]);
    printf("Descriptor en el hijo después de execv (ARG): %d\n", fd);

    /* Consulta las banderas del descriptor. */
    int flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        perror("fcntl");
        return 1;
    }

    if (flags & FD_CLOEXEC) {
        printf("FD_CLOEXEC está ACTIVADO\n");
    } else {
        printf("FD_CLOEXEC está DESACTIVADO\n");
    }

    char buffer[100];

    /* Lee directamente del número de descriptor recibido por argv. */
    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
    printf("n = %zd\n", n);

    if (n == -1) {
        perror("read");
        return 1;
    }

    if (n > 0) {
        buffer[n] = '\0';
        printf("Recibido: %s", buffer);
    }

    close(fd);
    return 0;
}
