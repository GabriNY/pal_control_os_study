#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 0x5203
#define SEM_KEY 0x6203
#define HIJOS 4              /* Padre + 4 hijos = 5 procesos totales. */
#define REPETICIONES 5000

union semun { int val; struct semid_ds *buf; unsigned short *array; };
static struct sembuf P = {0, -1, 0};
static struct sembuf V = {0, +1, 0};
static void down(int id) { if (semop(id, &P, 1) == -1) { perror("P"); _exit(1); } }
static void up(int id) { if (semop(id, &V, 1) == -1) { perror("V"); _exit(1); } }

int main(void)
{
    int shmid = shmget(SHM_KEY, sizeof(int), IPC_CREAT | 0666);
    int semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    union semun arg;
    int *contador;

    if (shmid == -1 || semid == -1) { perror("IPC"); return 1; }
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) { perror("SETVAL"); return 1; }
    contador = shmat(shmid, NULL, 0);
    if (contador == (void *)-1) { perror("shmat"); return 1; }
    *contador = 0;

    printf("[A padre PID=%d] crea 4 hijos => 5 procesos totales\n", getpid());
    fflush(stdout);

    for (int i = 0; i < HIJOS; ++i) {
        pid_t pid = fork();
        if (pid == -1) { perror("fork"); return 1; }
        if (pid == 0) {
            for (int j = 0; j < REPETICIONES; ++j) {
                down(semid);
                ++(*contador);
                up(semid);
            }
            printf("[proceso %c PID=%d] listo\n", 'B' + i, getpid());
            fflush(stdout);
            shmdt(contador);
            _exit(0);
        }
    }

    for (int i = 0; i < HIJOS; ++i) wait(NULL);
    printf("[A] contador final=%d; esperado=%d\n", *contador, HIJOS * REPETICIONES);

    shmdt(contador);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    return 0;
}
