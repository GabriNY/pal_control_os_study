/*
 * ARCHIVO: 01_basics/08_dup2_exec_child.c
 * TEMAS: dup2(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * dup2 redirige un descriptor heredado a STDIN/STDOUT; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    char buf[200];
    if (fgets(buf, sizeof(buf), stdin)) printf("Programa ejecutado recibio: %s", buf);
    return 0;
}
