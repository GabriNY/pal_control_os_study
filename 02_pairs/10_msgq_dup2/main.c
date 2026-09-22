/*
 * ARCHIVO: 02_pairs/10_msgq_dup2/main.c
 * TEMAS: FIFO (named pipe), colas de mensajes System V, dup2(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * el FIFO comunica procesos mediante una ruta visible en /tmp; la cola de mensajes transporta mensajes tipados a través del kernel; dup2 redirige un descriptor heredado a STDIN/STDOUT; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define KEY 0x5210
/* Define la estructura del mensaje; en System V el primer campo debe ser long y actúa como tipo. */
struct msg {
    long type;
    char text[128];
};
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Crea u obtiene la cola de mensajes identificada por la clave KEY. */
    int q = msgget(KEY, IPC_CREAT|0666);
    struct msg s = {
        .type = 1
    }
    , r;
    strcpy(s.text, "Mensaje que sera impreso a archivo");
    /* Envía a la cola el cuerpo del mensaje; mtype/type permite separar clases de mensajes. */
    msgsnd(q, &s, strlen(s.text) + 1, 0);
    /* Espera y recibe de la cola un mensaje del tipo solicitado; puede bloquear si todavía no existe. */
    msgrcv(q, &r, sizeof(r.text), 1, 0);
    /* Abre el canal por escritura. En un FIFO puede bloquear hasta que exista un lector. */
    int f = open("mq_dup2.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (f == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("open");
        return 1;
    }
    /* Redirige un descriptor conocido (normalmente STDIN=0 o STDOUT=1) hacia el recurso indicado. */
    dup2(f, STDOUT_FILENO);
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(f);
    printf("%s\n", r.text);
    return 0;
}
