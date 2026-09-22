/*
 * ARCHIVO: 01_basics/06_msg_receiver.c
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
#include <sys/ipc.h>
#include <sys/msg.h>
#define KEY 0x4512
/* Define la estructura del mensaje; en System V el primer campo debe ser long y actúa como tipo. */
struct msg {
    long type;
    char text[128];
};
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Crea u obtiene la cola de mensajes identificada por la clave KEY. */
    int q = msgget(KEY, IPC_CREAT | 0666);
    if (q == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("msgget");
        return 1;
    }
    struct msg m;
    /* Espera y recibe de la cola un mensaje del tipo solicitado; puede bloquear si todavía no existe. */
    if (msgrcv(q, &m, sizeof(m.text), 1, 0) == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("msgrcv");
        return 1;
    }
    printf("Tipo=%ld, texto=%s\n", m.type, m.text);
    return 0;
}
