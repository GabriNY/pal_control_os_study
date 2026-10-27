#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 0x5205
#define SEM_KEY 0x6205
#define HIJOS 4
#define SEM_INICIO 0
#define SEM_MUTEX  1

static volatile sig_atomic_t iniciar = 0;

static void handler(int sig)
{
    (void)sig;
    iniciar = 1;
}

static void sem_op(int id, unsigned short num, short delta)
{
    struct sembuf sb = {num, delta, 0};
    if (semop(id, &sb, 1) == -1) {
        perror("semop");
        _exit(1);
    }
}

union semun { int val; struct semid_ds *buf; unsigned short *array; };

int main(void)
{
    int shmid = shmget(SHM_KEY, sizeof(int), IPC_CREAT | 0666);
    int semid = semget(SEM_KEY, 2, IPC_CREAT | 0666);
    int *contador;
    union semun arg;
    unsigned short valores[2] = {0, 1};

    if (shmid == -1 || semid == -1) {
        perror("crear IPC");
        return 1;
    }

    /*
     * SEM_INICIO = 0: todos los hijos empiezan bloqueados.
     * SEM_MUTEX  = 1: protege el contador en memoria compartida.
     */
    arg.array = valores;
    if (semctl(semid, 0, SETALL, arg) == -1) {
        perror("semctl SETALL");
        return 1;
    }

    contador = shmat(shmid, NULL, 0);
    if (contador == (void *)-1) {
        perror("shmat");
        return 1;
    }
    *contador = 0;

    signal(SIGUSR1, handler);

    for (int i = 0; i < HIJOS; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            /* 1) Espera a que SIGUSR1 haga que el padre abra la compuerta. */
            sem_op(semid, SEM_INICIO, -1);

            /* 2) Exclusión mutua real sobre el contador compartido. */
            sem_op(semid, SEM_MUTEX, -1);
            ++(*contador);
            printf("[hijo PID=%d] contador=%d\n", getpid(), *contador);
            fflush(stdout);
            sem_op(semid, SEM_MUTEX, +1);

            shmdt(contador);
            _exit(0);
        }
    }

    printf("[padre PID=%d] envia desde otra terminal: kill -USR1 %d\n",
           getpid(), getpid());
    fflush(stdout);

    while (!iniciar) {
        pause();
    }

    /* Cuatro V sobre SEM_INICIO permiten continuar a cuatro hijos. */
    for (int i = 0; i < HIJOS; ++i) {
        sem_op(semid, SEM_INICIO, +1);
    }

    for (int i = 0; i < HIJOS; ++i) {
        wait(NULL);
    }

    printf("[padre] contador final=%d\n", *contador);

    shmdt(contador);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    return 0;
}
