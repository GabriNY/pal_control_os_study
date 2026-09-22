/*
 * ARCHIVO: 02_pairs/21_dup2_execv/main.c
 * TEMAS: FIFO (named pipe), dup2(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * el FIFO comunica procesos mediante una ruta visible en /tmp; dup2 redirige un descriptor heredado a STDIN/STDOUT; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Abre el canal por escritura. En un FIFO puede bloquear hasta que exista un lector. */
    int f = open("exec_output.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644);
    if (f == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("open");
        return 1;
    }
    /* Redirige un descriptor conocido (normalmente STDIN=0 o STDOUT=1) hacia el recurso indicado. */
    dup2(f, STDOUT_FILENO);
    /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
    close(f);
    char * a[] = {
        "/bin/echo", "stdout de execv redirigido con dup2", NULL
    };
    /* Reemplaza la imagen del proceso actual por otro ejecutable; si funciona, no retorna a esta línea. */
    execv(a[0], a);
    /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
    perror("execv");
    return 1;
}
