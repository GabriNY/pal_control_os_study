/*
 * ARCHIVO: 04_quads/02_signal_pipe_fork_dup2/main.c
 * TEMAS: señales POSIX, pipe anónimo, fork(), dup2(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * la señal inicia o habilita una acción sin transportar el dato principal; el pipe mueve bytes mediante fd[1] -> fd[0]; fork crea el proceso hijo y hereda los descriptores ya abiertos; dup2 redirige un descriptor heredado a STDIN/STDOUT; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
/* Bandera segura para comunicar al flujo principal que llegó una señal; el handler debe hacer el mínimo trabajo posible. */
static volatile sig_atomic_t go = 0;
/* Manejador de señal: se ejecuta cuando el proceso recibe la señal registrada. */
static void h(int s) {
    (void)s;
    go = 1;
}
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Arreglo de dos descriptores usado por pipe(): índice 0 lee e índice 1 escribe. */
    int fd[2];
    /* Crea una tubería anónima: fd[0] es lectura y fd[1] es escritura. Conviene crearla antes de fork(). */
    pipe(fd);
    /* Duplica el proceso. El hijo obtiene 0; el padre obtiene el PID del hijo y ambos heredan descriptores abiertos. */
    pid_t p = fork();
    /* Rama del proceso hijo: aquí se configura su comunicación o se prepara un execv(). */
    if (p == 0) {
        /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
        close(fd[1]);
        /* Redirige un descriptor conocido (normalmente STDIN=0 o STDOUT=1) hacia el recurso indicado. */
        dup2(fd[0], STDIN_FILENO);
        /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
        close(fd[0]);
        char b[128];
        if (fgets(b, sizeof(b), stdin))printf("Hijo stdin: %s", b);
        return 0;
    }
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(fd[0]);
    /* Asocia una señal con el manejador que cambiará el estado del programa. */
    signal(SIGUSR1, h);
    /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
    printf("PID=%d; kill -USR1 %d\n", getpid(), getpid());
    /* Pone al proceso en espera pasiva hasta que llegue una señal. */
    while (!go)pause();
    /* Escribe bytes en el descriptor correspondiente y alimenta el siguiente punto de la comunicación. */
    write(fd[1], "signal->pipe->dup2\n", 19);
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(fd[1]);
    /* Sincroniza al padre con sus hijos y evita procesos zombie. */
    wait(NULL);
    return 0;
}
