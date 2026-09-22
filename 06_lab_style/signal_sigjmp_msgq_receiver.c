/*
 * ARCHIVO: 06_lab_style/signal_sigjmp_msgq_receiver.c
 * TEMAS: señales POSIX, sigsetjmp/siglongjmp, colas de mensajes System V, execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * la señal inicia o habilita una acción sin transportar el dato principal; la cola de mensajes transporta mensajes tipados a través del kernel; execv reemplaza el programa actual sin crear un PID nuevo; sigsetjmp/siglongjmp permite retornar a un punto guardado tras una señal.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define KEY 0x5601
/* Define la estructura del mensaje; en System V el primer campo debe ser long y actúa como tipo. */
struct msg {
    long type;
    char text[128];
};
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Crea u obtiene la cola de mensajes identificada por la clave KEY. */
    int q = msgget(KEY, IPC_CREAT|0666);
    for (;;) {
        struct msg m;
        /* Espera y recibe de la cola un mensaje del tipo solicitado; puede bloquear si todavía no existe. */
        if (msgrcv(q, &m, sizeof(m.text), 0, 0) == -1) {
            /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
            perror("msgrcv");
            return 1;
        }
        printf("Tipo %ld: %s\n", m.type, m.text);
    }
}
