#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 0x5206
#define SEM_KEY 0x6206
#define HIJOS 4

union semun { int val; struct semid_ds *buf; unsigned short *array; };

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

    /* A crea cuatro procesos; cada hijo se transforma en worker con execv. */
    for (int i = 0; i < HIJOS; ++i) {
        pid_t pid = fork();
        if (pid == -1) { perror("fork"); return 1; }
        if (pid == 0) {
            char indice[16];
            snprintf(indice, sizeof(indice), "%d", i);
            char *args[] = {"./worker_shm_sem", indice, NULL};
            execv(args[0], args);
            perror("execv");
            _exit(1);
        }
    }

    for (int i = 0; i < HIJOS; ++i) wait(NULL);
    printf("[coordinador] contador final=%d esperado=%d\n", *contador, HIJOS);

    shmdt(contador);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    return 0;
}
