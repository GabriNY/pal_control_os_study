/*
 * ARCHIVO: 05_all_topics/worker.c
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
#include <string.h>
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
    char buf[256];
    if (!fgets(buf, sizeof(buf), stdin)) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("fgets");
        return 1;
    }
    printf("[worker] Recibi por STDIN/pipe: %s", buf);
    /* Crea u obtiene la cola de mensajes identificada por la clave KEY. */
    int q = msgget(KEY, IPC_CREAT|0666);
    if (q == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("msgget");
        return 1;
    }
    struct msg m = {
        .type = 7
    };
    strncpy(m.text, buf, sizeof(m.text)-1);
    m.text[sizeof(m.text)-1] = '\0';
    /* Envía a la cola el cuerpo del mensaje; mtype/type permite separar clases de mensajes. */
    if (msgsnd(q, &m, strlen(m.text) + 1, 0) == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("msgsnd");
        return 1;
    }
    printf("[worker] Enviado a Message Queue, type=7\n");
    return 0;
}
