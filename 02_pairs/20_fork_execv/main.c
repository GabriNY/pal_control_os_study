/*
 * ARCHIVO: 02_pairs/20_fork_execv/main.c
 * TEMAS: fork(), execv()
 *
 * RELACIÓN DENTRO DEL EJEMPLO:
 * fork crea el proceso hijo y hereda los descriptores ya abiertos; execv reemplaza el programa actual sin crear un PID nuevo.
 *
 * REGLA DE ESTUDIO: identifica primero quién crea el recurso, quién escribe,
 * quién lee, qué descriptores se heredan y qué proceso cambia con execv().
 */

/* Cabeceras necesarias para E/S, procesos, señales e IPC usados por este archivo. */
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
/* Punto de entrada del programa; desde aquí se crean recursos y se organiza el flujo del ejemplo. */
int main(void) {
    /* Duplica el proceso. El hijo obtiene 0; el padre obtiene el PID del hijo y ambos heredan descriptores abiertos. */
    pid_t p = fork();
    if (p == -1) {
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("fork");
        return 1;
    }
    /* Rama del proceso hijo: aquí se configura su comunicación o se prepara un execv(). */
    if (p == 0) {
        char * a[] = {
            "./target", NULL
        };
        /* Reemplaza la imagen del proceso actual por otro ejecutable; si funciona, no retorna a esta línea. */
        execv(a[0], a);
        /* Muestra el motivo del último error del sistema usando errno; facilita depurar la llamada que falló. */
        perror("execv");
        _exit(1);
    }
    /* Sincroniza al padre con sus hijos y evita procesos zombie. */
    wait(NULL);
    puts("Padre sigue vivo");
    return 0;
}
