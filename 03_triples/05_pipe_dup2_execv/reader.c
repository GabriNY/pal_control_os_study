/*
 * ARCHIVO: 03_triples/05_pipe_dup2_execv/reader.c
 * TEMAS: pipe anónimo, dup2(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * el pipe mueve bytes mediante fd[1] -> fd[0]; dup2 redirige un descriptor heredado a STDIN/STDOUT; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    char b[128];
    if (fgets(b, sizeof(b), stdin))printf("reader: %s", b);
    return 0;
}
