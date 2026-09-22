/*
 * ARCHIVO: 04_quads/08_signal_msgq_fork_execv/main.c
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
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define KEY 0x5408
/* Define la estructura del mensaje; en System V el primer campo debe ser long y actúa como tipo. */
struct msg {
    long type;
    char text[128];
};
/* Bandera segura para comunicar al flujo principal que llegó una señal; el handler debe hacer el mínimo trabajo posible. */
static volatile sig_atomic_t go = 0;
/* Manejador de señal: se ejecuta cuando el proceso recibe la señal registrada. */
static void h(int s) {
    (void)s;
    go = 1;
}
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Crea u obtiene la cola de mensajes identificada por la clave KEY. */
    int q = msgget(KEY, IPC_CREAT|0666);
    /* Duplica el proceso. El hijo obtiene 0; el padre obtiene el PID del hijo y ambos heredan descriptores abiertos. */
    pid_t p = fork();
    /* Rama del proceso hijo: aquí se configura su comunicación o se prepara un execv(). */
    if (p == 0) {
        struct msg m;
        /* Espera y recibe de la cola un mensaje del tipo solicitado; puede bloquear si todavía no existe. */
        if (msgrcv(q, &m, sizeof(m.text), 1, 0) == -1)_exit(1);
        char * a[] = {
            "./printer", m.text, NULL
        };
        /* Reemplaza la imagen del proceso actual por otro ejecutable; si funciona, no retorna a esta línea. */
        execv(a[0], a);
        _exit(1);
    }
    /* Asocia una señal con el manejador que cambiará el estado del programa. */
    signal(SIGUSR1, h);
    /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
    printf("PID=%d; kill -USR1 %d\n", getpid(), getpid());
    /* Pone al proceso en espera pasiva hasta que llegue una señal. */
    while (!go)pause();
    struct msg m = {
        .type = 1
    };
    strcpy(m.text, "Signal -> MQ -> hijo -> execv");
    /* Envía a la cola el cuerpo del mensaje; mtype/type permite separar clases de mensajes. */
    msgsnd(q, &m, strlen(m.text) + 1, 0);
    /* Sincroniza al padre con sus hijos y evita procesos zombie. */
    wait(NULL);
    return 0;
}
