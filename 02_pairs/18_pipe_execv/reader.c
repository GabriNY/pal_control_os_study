/*
 * ARCHIVO: 02_pairs/18_pipe_execv/reader.c
 * TEMAS: pipe anónimo, execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * el pipe mueve bytes mediante fd[1] -> fd[0]; execv reemplaza el programa actual sin crear un PID nuevo.
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
        printf("%s", b);
    }
    return 0;
}
