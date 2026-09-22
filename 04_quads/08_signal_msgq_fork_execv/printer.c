/*
 * ARCHIVO: 04_quads/08_signal_msgq_fork_execv/printer.c
 * TEMAS: señales POSIX, colas de mensajes System V, fork(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * la señal inicia o habilita una acción sin transportar el dato principal; la cola de mensajes transporta mensajes tipados a través del kernel; fork crea el proceso hijo y hereda los descriptores ya abiertos; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(int c, char * * v) {
    printf("%s\n", c > 1?v[1]:"-");
    return 0;
}
