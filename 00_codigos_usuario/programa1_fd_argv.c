/*
 * PROGRAMA 1 - Variante Pipe + fork + execv pasando el FD por argv.
 * Diferencia con dup2: el nuevo programa conoce el número exacto del descriptor
 * y ejecuta read(fd, ...) en lugar de read(STDIN_FILENO, ...).
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        /* HIJO: no escribe al Pipe. */
        close(fd[1]);

        char descriptor[20];

        /* execv solo pasa strings por argv, por eso convertimos fd[0] a texto. */
        snprintf(descriptor, sizeof(descriptor), "%d", fd[0]);

        char *args[] = {
            "./programa3",
            descriptor,
            NULL
        };

        printf("Descriptor heredado que se pasará a programa3: %s\n", descriptor);
        fflush(stdout); /* Fuerza la impresión antes de reemplazar el programa. */

        /* El descriptor fd[0] seguirá abierto si FD_CLOEXEC está desactivado. */
        execv(args[0], args);

        perror("execv");
        _exit(1);
    }

    /* PADRE: solo escribe. */
    close(fd[0]);

    char mensaje[] = "Keiko y Pedrito\n";
    if (write(fd[1], mensaje, sizeof(mensaje) - 1) == -1) {
        perror("write");
    }

    close(fd[1]);
    waitpid(pid, NULL, 0);
    return 0;
}
