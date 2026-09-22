/*
 * ARCHIVO: 01_basics/01_signal_flags.c
 * TEMAS: señales POSIX, execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * la señal inicia o habilita una acción sin transportar el dato principal; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
/* Bandera segura para comunicar al flujo principal que llegó una señal; el handler debe hacer el mínimo trabajo posible. */
static volatile sig_atomic_t got_usr1 = 0;
/* Manejador de señal: se ejecuta cuando el proceso recibe la señal registrada. */
static void handler(int sig) {
    (void)sig;
    got_usr1 = 1;
}
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    /* Inicializa vacía la máscara de señales bloqueadas mientras corre el handler. */
    sigemptyset(&sa.sa_mask);
    /* Registra formalmente el manejador de una señal mediante sigaction(). */
    sigaction(SIGUSR1, &sa, NULL);
    /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
    printf("PID = %d\n", getpid());
    /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
    printf("En otra terminal: kill -USR1 %d\n", getpid());
    for (;;) {
        /* Pone al proceso en espera pasiva hasta que llegue una señal. */
        pause();
        if (got_usr1) {
            got_usr1 = 0;
            printf("SIGUSR1 procesada fuera del handler\n");
        }
    }
}
