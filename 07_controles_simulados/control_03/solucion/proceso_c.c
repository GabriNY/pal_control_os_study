/* CONTROL 03 - C no conoce el fd original: lee descriptor estándar 0. */
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    char buffer[256];
    ssize_t n = read(STDIN_FILENO, buffer, sizeof(buffer) - 1);

    if (n == -1) {
        perror("read STDIN");
        return 1;
    }

    if (n > 0) {
        buffer[n] = '\0';
        printf("[C PID=%d] leído desde STDIN después de dup2: %s",
               getpid(), buffer);
    }
    return 0;
}
