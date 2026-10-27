/*
 * Basado en 11_semaphoreinit.c.
 * Crea un conjunto con 1 semáforo y lo inicializa en 1.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SEM_KEY 0x4444

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

int main(void)
{
    int semid;
    union semun arg;

    semid = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
    if (semid == -1) {
        perror("semget");
        return 1;
    }

    /* Valor 1 => recurso disponible para exclusión mutua. */
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl SETVAL");
        return 1;
    }

    printf("Semaforo inicializado. key=0x%x semid=%d valor=1\n", SEM_KEY, semid);
    printf("Comprueba con: ipcs -s\n");
    return 0;
}
