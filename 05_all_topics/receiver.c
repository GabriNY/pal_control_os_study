/*
 * ARCHIVO: 05_all_topics/receiver.c
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
#define KEY 0x5A77
/* Define la estructura del mensaje; en System V el primer campo debe ser long y actúa como tipo. */
struct msg {
    long type;
    char text[256];
};
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Crea u obtiene la cola de mensajes identificada por la clave KEY. */
    int q = msgget(KEY, IPC_CREAT|0666);
    if (q == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("msgget");
        return 1;
    }
    printf("[receiver] Esperando type=7...\n");
    struct msg m;
    /* Espera y recibe de la cola un mensaje del tipo solicitado; puede bloquear si todavía no existe. */
    if (msgrcv(q, &m, sizeof(m.text), 7, 0) == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("msgrcv");
        return 1;
    }
    printf("[receiver] Mensaje final: %s", m.text);
    /* Elimina la cola del kernel al finalizar para no dejar recursos IPC residuales. */
    msgctl(q, IPC_RMID, NULL);
    return 0;
}
