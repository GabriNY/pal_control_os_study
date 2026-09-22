/*
 * ARCHIVO: 01_basics/08_dup2_exec_parent.c
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
#include <stdlib.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Arreglo de dos descriptores usado por pipe(): índice 0 lee e índice 1 escribe. */
    int fd[2];
    /* Crea una tubería anónima: fd[0] es lectura y fd[1] es escritura. Conviene crearla antes de fork(). */
    if (pipe(fd) == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("pipe");
        return 1;
    }
    /* Duplica el proceso. El hijo obtiene 0; el padre obtiene el PID del hijo y ambos heredan descriptores abiertos. */
    pid_t pid = fork();
    if (pid == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("fork");
        return 1;
    }
    /* Rama del proceso hijo: aquí se configura su comunicación o se prepara un execv(). */
    if (pid == 0) {
        /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
        close(fd[1]);
        /* Redirige un descriptor conocido (normalmente STDIN=0 o STDOUT=1) hacia el recurso indicado. */
        if (dup2(fd[0], STDIN_FILENO) == -1) {
            /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
            perror("dup2");
            _exit(1);
        }
        /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
        close(fd[0]);
        char * args[] = {
            "./08_dup2_exec_child", NULL
        };
        /* Reemplaza la imagen del proceso actual por otro ejecutable; si funciona, no retorna a esta línea. */
        execv(args[0], args);
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("execv");
        _exit(1);
    }
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(fd[0]);
    const char * msg = "Mensaje que termina como STDIN del programa ejecutado\n";
    /* Escribe bytes en el descriptor correspondiente y alimenta el siguiente punto de la comunicación. */
    write(fd[1], msg, strlen(msg));
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(fd[1]);
    /* Sincroniza al padre con sus hijos y evita procesos zombie. */
    wait(NULL);
    return 0;
}
