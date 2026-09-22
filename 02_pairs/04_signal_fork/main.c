/*
 * ARCHIVO: 02_pairs/04_signal_fork/main.c
 * TEMAS: señales POSIX, fork(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * la señal inicia o habilita una acción sin transportar el dato principal; fork crea el proceso hijo y hereda los descriptores ya abiertos; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
/* Bandera segura para comunicar al flujo principal que llegó una señal; el handler debe hacer el mínimo trabajo posible. */
static volatile sig_atomic_t go = 0;
/* Manejador de señal: se ejecuta cuando el proceso recibe la señal registrada. */
static void h(int s) {
    (void)s;
    go = 1;
}
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Asocia una señal con el manejador que cambiará el estado del programa. */
    signal(SIGUSR1, h);
    /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
    printf("PID=%d; kill -USR1 %d\n", getpid(), getpid());
    /* Pone al proceso en espera pasiva hasta que llegue una señal. */
    while (!go) pause();
    /* Duplica el proceso. El hijo obtiene 0; el padre obtiene el PID del hijo y ambos heredan descriptores abiertos. */
    pid_t p = fork();
    if (p == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("fork");
        return 1;
    }
    /* Rama del proceso hijo: aquí se configura su comunicación o se prepara un execv(). */
    if (p == 0) {
        /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
        printf("Soy hijo PID=%d, PPID=%d\n", getpid(), getppid());
        return 0;
    }
    /* Sincroniza al padre con sus hijos y evita procesos zombie. */
    wait(NULL);
    puts("Padre termino de esperar al hijo");
    return 0;
}
