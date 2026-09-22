/* CONTROL 04 - C recibe el Pipe como STDIN y procesa dos líneas. */
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    char linea[256];

    for (int i = 0; i < 2; ++i) {
        if (fgets(linea, sizeof(linea), stdin) == NULL) {
            break;
        }
        printf("[C PID=%d] STDIN/Pipe: %s", getpid(), linea);
    }

    return 0;
}
