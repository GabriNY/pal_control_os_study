#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 0x5206
#define SEM_KEY 0x6206

static void op(int id, short delta) {
    struct sembuf sb = {0, delta, 0};
    if (semop(id, &sb, 1) == -1) { perror("semop"); exit(1); }
}

int main(int argc, char *argv[])
{
    if (argc != 2) { fprintf(stderr, "Uso interno: worker indice\n"); return 1; }

    int shmid = shmget(SHM_KEY, sizeof(int), 0666);
    int semid = semget(SEM_KEY, 1, 0);
    int *contador;
    if (shmid == -1 || semid == -1) { perror("obtener IPC"); return 1; }

    contador = shmat(shmid, NULL, 0);
    if (contador == (void *)-1) { perror("shmat"); return 1; }

    op(semid, -1);
    ++(*contador);
    printf("[worker %s PID=%d] contador=%d\n", argv[1], getpid(), *contador);
    op(semid, +1);

    shmdt(contador);
    return 0;
}
