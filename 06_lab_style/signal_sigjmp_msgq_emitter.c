/*
 * ARCHIVO: 06_lab_style/signal_sigjmp_msgq_emitter.c
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
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define KEY 0x5601
/* Define la estructura del mensaje; en System V el primer campo debe ser long y actúa como tipo. */
struct msg {
    long type;
    char text[128];
};
static sigjmp_buf j1, j2, j3;
static void h1(int s) {
    (void)s;
    /* Salta al contexto guardado por sigsetjmp(); se usa en el patrón trabajado en laboratorio. */
    siglongjmp(j1, 1);
}
static void h2(int s) {
    (void)s;
    /* Salta al contexto guardado por sigsetjmp(); se usa en el patrón trabajado en laboratorio. */
    siglongjmp(j2, 1);
}
static void h3(int s) {
    (void)s;
    /* Salta al contexto guardado por sigsetjmp(); se usa en el patrón trabajado en laboratorio. */
    siglongjmp(j3, 1);
}
static void sendm(int q, long t, const char * txt) {
    struct msg m = {
        .type = t
    };
    /* Convierte el número de descriptor a texto para pasarlo por argv a través de execv(). */
    snprintf(m.text, sizeof(m.text), "%s", txt);
    /* Envía a la cola el cuerpo del mensaje; mtype/type permite separar clases de mensajes. */
    if (msgsnd(q, &m, strlen(m.text) + 1, 0) == -1)perror("msgsnd");
}
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Crea u obtiene la cola de mensajes identificada por la clave KEY. */
    int q = msgget(KEY, IPC_CREAT|0666);
    /* Asocia una señal con el manejador que cambiará el estado del programa. */
    signal(SIGINT, h1);
    /* Asocia una señal con el manejador que cambiará el estado del programa. */
    signal(SIGUSR1, h2);
    /* Asocia una señal con el manejador que cambiará el estado del programa. */
    signal(SIGUSR2, h3);
    /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
    printf("PID=%d\n", getpid());
    for (;;) {
        /* Guarda el contexto de ejecución para poder regresar aquí mediante siglongjmp(). */
        if (sigsetjmp(j1, 1) != 0) {
            puts("Recuperacion SIGINT");
            sendm(q, 1, "Mensaje por SIGINT");
        }
        /* Guarda el contexto de ejecución para poder regresar aquí mediante siglongjmp(). */
        if (sigsetjmp(j2, 1) != 0) {
            puts("Recuperacion SIGUSR1");
            sendm(q, 2, "Mensaje por SIGUSR1");
        }
        /* Guarda el contexto de ejecución para poder regresar aquí mediante siglongjmp(). */
        if (sigsetjmp(j3, 1) != 0) {
            puts("Recuperacion SIGUSR2");
            sendm(q, 3, "Mensaje por SIGUSR2");
        }
        /* Pone al proceso en espera pasiva hasta que llegue una señal. */
        pause();
    }
}
