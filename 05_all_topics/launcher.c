/*
 * ARCHIVO: 05_all_topics/launcher.c
 * TEMAS: señales POSIX, FIFO (named pipe), pipe anónimo, fork(), dup2(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * la señal inicia o habilita una acción sin transportar el dato principal; el FIFO comunica procesos mediante una ruta visible en /tmp; el pipe mueve bytes mediante fd[1] -> fd[0]; fork crea el proceso hijo y hereda los descriptores ya abiertos; dup2 redirige un descriptor heredado a STDIN/STDOUT; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#define FIFO "/tmp/os_exam_all_fifo"
/* Bandera segura para comunicar al flujo principal que llegó una señal; el handler debe hacer el mínimo trabajo posible. */
static volatile sig_atomic_t go = 0;
/* Manejador de señal: se ejecuta cuando el proceso recibe la señal registrada. */
static void h(int sig) {
    (void)sig;
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
    /* Arreglo de dos descriptores usado por pipe(): índice 0 lee e índice 1 escribe. */
    int pfd[2];
    /* Crea una tubería anónima: fd[0] es lectura y fd[1] es escritura. Conviene crearla antes de fork(). */
    if (pipe(pfd) == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("pipe");
        return 1;
    }
    /* Duplica el proceso. El hijo obtiene 0; el padre obtiene el PID del hijo y ambos heredan descriptores abiertos. */
    pid_t pid = fork();
    if (pid == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("fork");
        return 1;
    }
    /* Rama del proceso hijo: aquí se configura su comunicación o se prepara un execv(). */
    if (pid == 0) {
        /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
        close(pfd[1]);
        /* Redirige un descriptor conocido (normalmente STDIN=0 o STDOUT=1) hacia el recurso indicado. */
        if (dup2(pfd[0], STDIN_FILENO) == -1) {
            /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
            perror("dup2");
            _exit(1);
        }
        /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
        close(pfd[0]);
        char * args[] = {
            "./worker", NULL
        };
        /* Reemplaza la imagen del proceso actual por otro ejecutable; si funciona, no retorna a esta línea. */
        execv(args[0], args);
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("execv");
        _exit(1);
    }
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(pfd[0]);
    /* Asocia una señal con el manejador que cambiará el estado del programa. */
    signal(SIGUSR1, h);
    /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
    printf("[launcher] PID padre = %d\n", getpid());
    /* Obtiene el PID actual; sirve para identificar el proceso y enviarle señales desde otra terminal. */
    printf("[launcher] 1) kill -USR1 %d\n", getpid());
    printf("[launcher] 2) en otra terminal: echo 'mensaje examen' > %s\n", FIFO);
    /* Pone al proceso en espera pasiva hasta que llegue una señal. */
    while (!go) pause();
    go = 0;
    printf("[launcher] Señal recibida. Abriendo FIFO...\n");
    /* Abre el canal por lectura. En un FIFO puede bloquear hasta que exista un escritor. */
    int f = open(FIFO, O_RDONLY);
    if (f == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("open FIFO");
        return 1;
    }
    char buf[256];
    /* Lee bytes desde el descriptor. El valor retornado indica cuántos bytes llegaron, 0 indica EOF y -1 error. */
    ssize_t n = read(f, buf, sizeof(buf)-1);
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(f);
    if (n <= 0) {
        fprintf(stderr, "No se leyeron datos del FIFO\n");
        return 1;
    }
    buf[n] = '\0';
    printf("[launcher] FIFO -> \"%s\"", buf);
    /* Escribe bytes en el descriptor correspondiente y alimenta el siguiente punto de la comunicación. */
    if (write(pfd[1], buf, (size_t)n) != n) perror("write pipe");
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(pfd[1]);
    /* Sincroniza al padre con sus hijos y evita procesos zombie. */
    wait(NULL);
    printf("[launcher] Worker termino.\n");
    return 0;
}
