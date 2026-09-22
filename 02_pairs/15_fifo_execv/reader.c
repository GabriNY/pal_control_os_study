/*
 * ARCHIVO: 02_pairs/15_fifo_execv/reader.c
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
#include <stdlib.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(int c, char * * v) {
    if (c != 2)return 1;
    /* Convierte el descriptor recibido por argv (texto) nuevamente a entero para poder usar read/close. */
    int f = atoi(v[1]);
    char b[128];
    /* Lee bytes desde el descriptor. El valor retornado indica cuántos bytes llegaron, 0 indica EOF y -1 error. */
    ssize_t n = read(f, b, 127);
    if (n > 0) {
        b[n] = '\0';
        printf("FD heredado por execv: %s", b);
    }
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(f);
    return 0;
}
