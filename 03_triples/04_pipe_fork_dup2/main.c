/*
 * ARCHIVO: 03_triples/04_pipe_fork_dup2/main.c
 * TEMAS: pipe anónimo, fork(), dup2(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * el pipe mueve bytes mediante fd[1] -> fd[0]; fork crea el proceso hijo y hereda los descriptores ya abiertos; dup2 redirige un descriptor heredado a STDIN/STDOUT; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Arreglo de dos descriptores usado por pipe(): índice 0 lee e índice 1 escribe. */
    int fd[2];
    /* Crea una tubería anónima: fd[0] es lectura y fd[1] es escritura. Conviene crearla antes de fork(). */
    pipe(fd);
    /* Duplica el proceso. El hijo obtiene 0; el padre obtiene el PID del hijo y ambos heredan descriptores abiertos. */
    pid_t p = fork();
    /* Rama del proceso hijo: aquí se configura su comunicación o se prepara un execv(). */
    if (p == 0) {
        /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
        close(fd[1]);
        /* Redirige un descriptor conocido (normalmente STDIN=0 o STDOUT=1) hacia el recurso indicado. */
        dup2(fd[0], STDIN_FILENO);
        /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
        close(fd[0]);
        char b[128];
        if (fgets(b, sizeof(b), stdin))printf("Hijo por stdin: %s", b);
        return 0;
    }
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(fd[0]);
    /* Escribe bytes en el descriptor correspondiente y alimenta el siguiente punto de la comunicación. */
    write(fd[1], "pipe+fork+dup2\n", 15);
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(fd[1]);
    /* Sincroniza al padre con sus hijos y evita procesos zombie. */
    wait(NULL);
    return 0;
}
