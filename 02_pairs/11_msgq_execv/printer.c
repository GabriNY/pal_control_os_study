/*
 * ARCHIVO: 02_pairs/11_msgq_execv/printer.c
 * TEMAS: colas de mensajes System V, execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * la cola de mensajes transporta mensajes tipados a través del kernel; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(int c, char * * v) {
    printf("printer: %s\n", c > 1?v[1]:"-");
    return 0;
}
