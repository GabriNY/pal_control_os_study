/*
 * Basado en el laboratorio 05.1 shm_server.c.
 * El servidor crea una memoria compartida, escribe a..z y espera que
 * el cliente cambie el primer byte a '*'.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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
    char c;

    /* Crea (o recupera si ya existe) el segmento System V. */
    shmid = shmget(SHM_KEY, SHMSZ, IPC_CREAT | 0666);
    if (shmid < 0) {
        perror("shmget");
        return 1;
    }

    /* Adjunta el segmento al espacio de direcciones de este proceso. */
    shm = shmat(shmid, NULL, 0);
    if (shm == (char *)-1) {
        perror("shmat");
        return 1;
    }

    /* La SHM se usa como un arreglo de chars compartido. */
    s = shm;
    for (c = 'a'; c <= 'z'; ++c) {
        *s++ = c;
    }
    *s = '\0';

    printf("[servidor PID=%d] SHMID=%d, esperando al cliente...\n", getpid(), shmid);

    /* Espera activa simple del laboratorio: '*' significa 'cliente terminó'. */
    while (*shm != '*') {
        sleep(1);
    }

    printf("[servidor] confirmacion recibida.\n");

    /* Se separa del segmento. */
    if (shmdt(shm) == -1) {
        perror("shmdt");
    }

    /* El servidor, como creador, elimina el recurso al final. */
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID");
    }

    return 0;
}
