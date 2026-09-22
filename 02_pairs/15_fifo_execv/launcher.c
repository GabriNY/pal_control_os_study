/*
 * ARCHIVO: 02_pairs/15_fifo_execv/launcher.c
 * TEMAS: FIFO (named pipe), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * el FIFO comunica procesos mediante una ruta visible en /tmp; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>
#define FIFO "/tmp/pair_fifo_execv"
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Crea el FIFO con nombre. Si ya existe, EEXIST no debe considerarse un error fatal. */
    if (mkfifo(FIFO, 0666) == -1 && errno != EEXIST) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("mkfifo");
        return 1;
    }
    printf("En otra terminal: echo hola > %s\n", FIFO);
    /* Abre el canal por lectura. En un FIFO puede bloquear hasta que exista un escritor. */
    int f = open(FIFO, O_RDONLY);
    if (f == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("open");
        return 1;
    }
    char n[32];
    /* Convierte el número de descriptor a texto para pasarlo por argv a través de execv(). */
    snprintf(n, sizeof(n), "%d", f);
    char * a[] = {
        "./reader", n, NULL
    };
    /* Reemplaza la imagen del proceso actual por otro ejecutable; si funciona, no retorna a esta línea. */
    execv(a[0], a);
    /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
    perror("execv");
    return 1;
}
