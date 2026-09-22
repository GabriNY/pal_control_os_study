/*
 * ARCHIVO: 03_triples/01_signal_fifo_msgq/bridge.c
 * TEMAS: señales POSIX, FIFO (named pipe), colas de mensajes System V, execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * la señal inicia o habilita una acción sin transportar el dato principal; el FIFO comunica procesos mediante una ruta visible en /tmp; la cola de mensajes transporta mensajes tipados a través del kernel; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <errno.h>
#define FIFO "/tmp/triple_sfm"
#define KEY 0x5301
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
    /* Crea el FIFO con nombre. Si ya existe, EEXIST no debe considerarse un error fatal. */
    if (mkfifo(FIFO, 0666) == -1 && errno != EEXIST) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("mkfifo");
        return 1;
    }
    /* Asocia una señal con el manejador que cambiará el estado del programa. */
    signal(SIGUSR1, h);
    /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
    printf("PID=%d. kill -USR1 %d, luego echo mensaje > %s\n", getpid(), getpid(), FIFO);
    /* Pone al proceso en espera pasiva hasta que llegue una señal. */
    while (!go) pause();
    /* Abre el canal por lectura. En un FIFO puede bloquear hasta que exista un escritor. */
    int f = open(FIFO, O_RDONLY);
    struct msg m = {
        .type = 3
    };
    /* Lee bytes desde el descriptor. El valor retornado indica cuántos bytes llegaron, 0 indica EOF y -1 error. */
    ssize_t n = read(f, m.text, 127);
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(f);
    if (n <= 0) return 1;
    m.text[n] = '\0';
    /* Crea u obtiene la cola de mensajes identificada por la clave KEY. */
    int q = msgget(KEY, IPC_CREAT|0666);
    /* Envía a la cola el cuerpo del mensaje; mtype/type permite separar clases de mensajes. */
    if (msgsnd(q, &m, strlen(m.text) + 1, 0) == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("msgsnd");
        return 1;
    }
    puts("Signal -> FIFO -> MQ");
    return 0;
}
