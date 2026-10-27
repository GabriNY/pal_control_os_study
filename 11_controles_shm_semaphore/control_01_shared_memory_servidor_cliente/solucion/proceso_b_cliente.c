/*
 * Basado en el laboratorio 05.2 shm_client.c.
 * El cliente localiza la SHM con la misma key, imprime su contenido y
 * escribe '*' en el primer byte para confirmar la lectura.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define SHMSZ 27
#define SHM_KEY 5678

int main(void)
{
    int shmid;
    char *shm;
    char *s;

    /* No usa IPC_CREAT: el servidor debe haber creado el segmento. */
    shmid = shmget(SHM_KEY, SHMSZ, 0666);
    if (shmid < 0) {
        perror("shmget");
        return 1;
    }

    shm = shmat(shmid, NULL, 0);
    if (shm == (char *)-1) {
        perror("shmat");
        return 1;
    }

    printf("[cliente] contenido: ");
    for (s = shm; *s != '\0'; ++s) {
        putchar(*s);
    }
    putchar('\n');

    /* Protocolo simple de confirmación del laboratorio. */
    *shm = '*';

    if (shmdt(shm) == -1) {
        perror("shmdt");
        return 1;
    }

    return 0;
}
