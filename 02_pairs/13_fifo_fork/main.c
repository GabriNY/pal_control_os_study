/*
 * ARCHIVO: 02_pairs/13_fifo_fork/main.c
 * TEMAS: FIFO (named pipe), fork(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * el FIFO comunica procesos mediante una ruta visible en /tmp; fork crea el proceso hijo y hereda los descriptores ya abiertos; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#define FIFO "/tmp/pair_fifo_fork"
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Crea el FIFO con nombre. Si ya existe, EEXIST no debe considerarse un error fatal. */
    if (mkfifo(FIFO, 0666) == -1 && errno != EEXIST) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("mkfifo");
        return 1;
    }
    /* Duplica el proceso. El hijo obtiene 0; el padre obtiene el PID del hijo y ambos heredan descriptores abiertos. */
    pid_t p = fork();
    /* Rama del proceso hijo: aquí se configura su comunicación o se prepara un execv(). */
    if (p == 0) {
        /* Abre el canal por lectura. En un FIFO puede bloquear hasta que exista un escritor. */
        int f = open(FIFO, O_RDONLY);
        char b[128];
        /* Lee bytes desde el descriptor. El valor retornado indica cuántos bytes llegaron, 0 indica EOF y -1 error. */
        ssize_t n = read(f, b, 127);
        if (n > 0) {
            b[n] = '\0';
            printf("Hijo: %s", b);
        }
        /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
        close(f);
        return 0;
    }
    /* Abre el canal por escritura. En un FIFO puede bloquear hasta que exista un lector. */
    int f = open(FIFO, O_WRONLY);
    const char * m = "Padre por FIFO\n";
    /* Escribe bytes en el descriptor correspondiente y alimenta el siguiente punto de la comunicación. */
    write(f, m, strlen(m));
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(f);
    /* Sincroniza al padre con sus hijos y evita procesos zombie. */
    wait(NULL);
    return 0;
}
