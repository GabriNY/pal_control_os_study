/*
 * ARCHIVO: 02_pairs/09_msgq_fork/main.c
 * TEMAS: colas de mensajes System V, fork(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * la cola de mensajes transporta mensajes tipados a través del kernel; fork crea el proceso hijo y hereda los descriptores ya abiertos; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define KEY 0x5209
/* Define la estructura del mensaje; en System V el primer campo debe ser long y actúa como tipo. */
struct msg {
    long type;
    char text[128];
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
    /* Duplica el proceso. El hijo obtiene 0; el padre obtiene el PID del hijo y ambos heredan descriptores abiertos. */
    pid_t p = fork();
    if (p == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("fork");
        return 1;
    }
    /* Rama del proceso hijo: aquí se configura su comunicación o se prepara un execv(). */
    if (p == 0) {
        struct msg m;
        /* Espera y recibe de la cola un mensaje del tipo solicitado; puede bloquear si todavía no existe. */
        if (msgrcv(q, &m, sizeof(m.text), 1, 0) == -1) {
            /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
            perror("msgrcv");
            return 1;
        }
        printf("Hijo recibio: %s\n", m.text);
        return 0;
    }
    sleep(1);
    struct msg m = {
        .type = 1
    };
    strcpy(m.text, "Padre -> MQ -> hijo");
    /* Envía a la cola el cuerpo del mensaje; mtype/type permite separar clases de mensajes. */
    msgsnd(q, &m, strlen(m.text) + 1, 0);
    /* Sincroniza al padre con sus hijos y evita procesos zombie. */
    wait(NULL);
    return 0;
}
