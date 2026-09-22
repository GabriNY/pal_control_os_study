/*
 * ARCHIVO: 02_pairs/18_pipe_execv/launcher.c
 * TEMAS: pipe anónimo, fork(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * el pipe mueve bytes mediante fd[1] -> fd[0]; fork crea el proceso hijo y hereda los descriptores ya abiertos; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Arreglo de dos descriptores usado por pipe(): índice 0 lee e índice 1 escribe. */
    int fd[2];
    /* Crea una tubería anónima: fd[0] es lectura y fd[1] es escritura. Conviene crearla antes de fork(). */
    pipe(fd);
    /* Escribe bytes en el descriptor correspondiente y alimenta el siguiente punto de la comunicación. */
    write(fd[1], "pipe heredado por execv\n", 24);
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(fd[1]);
    char n[32];
    /* Convierte el número de descriptor a texto para pasarlo por argv a través de execv(). */
    snprintf(n, sizeof(n), "%d", fd[0]);
    char * a[] = {
        "./reader", n, NULL
    };
    /* Reemplaza la imagen del proceso actual por otro ejecutable; si funciona, no retorna a esta línea. */
    execv(a[0], a);
    /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
    perror("execv");
    return 1;
}
