/*
 * ARCHIVO: 01_basics/04_fifo_reader.c
 * TEMAS: FIFO (named pipe), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * el FIFO comunica procesos mediante una ruta visible en /tmp; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#define FIFO "/tmp/os_exam_fifo"
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Crea el FIFO con nombre. Si ya existe, EEXIST no debe considerarse un error fatal. */
    if (mkfifo(FIFO, 0666) == -1 && errno != EEXIST) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("mkfifo");
        return 1;
    }
    /* Abre el canal por lectura. En un FIFO puede bloquear hasta que exista un escritor. */
    int fd = open(FIFO, O_RDONLY);
    if (fd == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("open");
        return 1;
    }
    char buf[128];
    /* Lee bytes desde el descriptor. El valor retornado indica cuántos bytes llegaron, 0 indica EOF y -1 error. */
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    if (n > 0) {
        buf[n] = '\0';
        printf("Recibido: %s", buf);
    }
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(fd);
    return 0;
}
