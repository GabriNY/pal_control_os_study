/*
 * ARCHIVO: 02_pairs/19_fork_dup2/main.c
 * TEMAS: FIFO (named pipe), fork(), dup2(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * el FIFO comunica procesos mediante una ruta visible en /tmp; fork crea el proceso hijo y hereda los descriptores ya abiertos; dup2 redirige un descriptor heredado a STDIN/STDOUT; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Duplica el proceso. El hijo obtiene 0; el padre obtiene el PID del hijo y ambos heredan descriptores abiertos. */
    pid_t p = fork();
    /* Rama del proceso hijo: aquí se configura su comunicación o se prepara un execv(). */
    if (p == 0) {
        /* Abre el canal por escritura. En un FIFO puede bloquear hasta que exista un lector. */
        int f = open("child_output.txt", O_WRONLY|O_CREAT|O_TRUNC, 0644);
        /* Redirige un descriptor conocido (normalmente STDIN=0 o STDOUT=1) hacia el recurso indicado. */
        dup2(f, STDOUT_FILENO);
        /* Cierra un descriptor que ya no se usa; esto evita fugas y ayuda a que los lectores detecten EOF. */
        close(f);
        printf("stdout del hijo redirigido\n");
        return 0;
    }
    /* Sincroniza al padre con sus hijos y evita procesos zombie. */
    wait(NULL);
    puts("Revisa child_output.txt");
    return 0;
}
