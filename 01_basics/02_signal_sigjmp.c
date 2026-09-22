/*
 * ARCHIVO: 01_basics/02_signal_sigjmp.c
 * TEMAS: señales POSIX, sigsetjmp/siglongjmp, execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * la señal inicia o habilita una acción sin transportar el dato principal; execv reemplaza el programa actual sin crear un PID nuevo; sigsetjmp/siglongjmp permite retornar a un punto guardado tras una señal.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>
static sigjmp_buf punto;
/* Manejador de señal: se ejecuta cuando el proceso recibe la señal registrada. */
static void handler(int sig) {
    (void)sig;
    /* Salta al contexto guardado por sigsetjmp(); se usa en el patrón trabajado en laboratorio. */
    siglongjmp(punto, 1);
}
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Asocia una señal con el manejador que cambiará el estado del programa. */
    signal(SIGUSR1, handler);
    /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
    printf("PID = %d\n", getpid());
    for (;;) {
        /* Guarda el contexto de ejecución para poder regresar aquí mediante siglongjmp(). */
        if (sigsetjmp(punto, 1) == 0) {
            printf("Esperando SIGUSR1...\n");
            /* Pone al proceso en espera pasiva hasta que llegue una señal. */
            pause();
        } else {
            /* Salta al contexto guardado por sigsetjmp(); se usa en el patrón trabajado en laboratorio. */
            printf("Regrese por siglongjmp(): señal procesada\n");
        }
    }
}
