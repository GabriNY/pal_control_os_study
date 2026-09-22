/*
 * ARCHIVO: 02_pairs/02_signal_fifo/main.c
 * TEMAS: señales POSIX, FIFO (named pipe), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * la señal inicia o habilita una acción sin transportar el dato principal; el FIFO comunica procesos mediante una ruta visible en /tmp; execv reemplaza el programa actual sin crear un PID nuevo.
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
#include <errno.h>
#define FIFO "/tmp/pair_signal_fifo"
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
    printf("PID=%d. Primero: kill -USR1 %d; luego: echo hola > %s\n", getpid(), getpid(), FIFO);
    for (;;) {
        /* Pone al proceso en espera pasiva hasta que llegue una señal. */
        pause();
        if (go) {
            go = 0;
            /* Abre el canal por lectura. En un FIFO puede bloquear hasta que exista un escritor. */
            int fd = open(FIFO, O_RDONLY);
            if (fd == -1) {
                /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
                perror("open");
                continue;
            }
            char b[128];
            /* Lee bytes desde el descriptor. El valor retornado indica cuántos bytes llegaron, 0 indica EOF y -1 error. */
            ssize_t n = read(fd, b, 127);
            /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
            close(fd);
            if (n > 0) {
                b[n] = '\0';
                printf("FIFO: %s", b);
            }
        }
    }
}
