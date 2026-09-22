/*
 * PROGRAMA 1 - Variante Pipe + fork + dup2 + execv.
 * Flujo: Padre --write(fd[1])--> Pipe --fd[0]--> Hijo --dup2--> STDIN --execv--> programa2.
 */
#include <stdio.h>      /* perror() */
#include <stdlib.h>     /* exit() */
#include <sys/types.h>  /* pid_t */
#include <sys/wait.h>   /* waitpid() */
#include <unistd.h>     /* pipe(), fork(), dup2(), execv(), read/write/close */

int main(void)
{
    int fd[2]; /* fd[0] = lectura; fd[1] = escritura. */

    /* Crear el Pipe antes de fork() permite que el hijo herede ambos extremos. */
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
        /* ================= PROCESO HIJO ================= */

        /* El hijo solo leerá, así que no necesita el extremo de escritura. */
        close(fd[1]);

        /* Hace que STDIN (descriptor 0) apunte al mismo Pipe que fd[0]. */
        if (dup2(fd[0], STDIN_FILENO) == -1) {
            perror("dup2");
            _exit(1);
        }

        /* Después de dup2(), fd[0] original puede cerrarse porque descriptor 0 ya apunta al Pipe. */
        close(fd[0]);

        /* argv del nuevo programa: argv[0] es el nombre y el último elemento siempre NULL. */
        char *args[] = {"./programa2", NULL};

        /* Reemplaza el código del hijo por programa2; el PID del hijo no cambia. */
        execv(args[0], args);

        /* Si execv funciona, esta línea nunca se ejecuta. */
        perror("execv");
        _exit(1);
    }

    /* ================= PROCESO PADRE ================= */

    /* El padre solo escribirá; cierra el extremo de lectura. */
    close(fd[0]);

    char mensaje[] = "Hola desde el proceso padre\n";

    /* Inserta los bytes en el Pipe para que el hijo/programa2 los lea por STDIN. */
    if (write(fd[1], mensaje, sizeof(mensaje) - 1) == -1) {
        perror("write");
    }

    /* Cerrar fd[1] permite que el lector detecte EOF cuando no queden escritores. */
    close(fd[1]);

    /* Espera la terminación del hijo y evita dejar un proceso zombie. */
    waitpid(pid, NULL, 0);
    return 0;
}
