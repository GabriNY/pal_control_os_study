/*
 * PROGRAMA 2 - Lector por STDIN.
 * Relación: este programa está pensado para ser ejecutado con execv() después
 * de que el proceso anterior haga dup2(fd[0], STDIN_FILENO). Por eso no necesita
 * conocer el número original del descriptor del Pipe.
 */
#include <stdio.h>   /* printf() */
#include <unistd.h>  /* read(), STDIN_FILENO */

int main(void)
{
    char buffer[100]; /* Espacio donde se guardarán los bytes recibidos. */
    ssize_t n;        /* read() retorna ssize_t porque también puede devolver -1. */

    /* Lee desde descriptor 0. Si el padre hizo dup2(), descriptor 0 apunta al Pipe. */
    n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);

    if (n == -1) {
        perror("read");
        return 1;
    }

    if (n > 0) {
        /* read() entrega bytes, no agrega '\0'; lo añadimos para usar %s. */
        buffer[n] = '\0';
        printf("Programa 2 recibió: %s", buffer);
    }

    return 0;
}
