/*
 * ARCHIVO: 02_pairs/20_fork_execv/target.c
 * TEMAS: fork(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * fork crea el proceso hijo y hereda los descriptores ya abiertos; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <unistd.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
    printf("Target PID=%d\n", getpid());
    return 0;
}
