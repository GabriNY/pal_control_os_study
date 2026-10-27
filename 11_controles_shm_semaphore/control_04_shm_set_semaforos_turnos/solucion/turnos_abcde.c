#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 0x5204
#define SEM_KEY 0x6204
#define N 5

typedef struct {
    int posicion;
    char orden[N + 1];
} Estado;

union semun { int val; struct semid_ds *buf; unsigned short *array; };

static void op(int semid, unsigned short num, short delta)
{
    struct sembuf sb = {num, delta, 0};
    if (semop(semid, &sb, 1) == -1) {
        perror("semop");
        exit(1);
    }
}

static void ejecutar_turno(int indice, int semid, Estado *e)
{
    /* P sobre el semáforo propio: espera hasta que sea su turno. */
    op(semid, (unsigned short)indice, -1);

    e->orden[e->posicion++] = (char)('A' + indice);
    e->orden[e->posicion] = '\0';
    printf("[proceso %c PID=%d] orden parcial=%s\n",
           'A' + indice, getpid(), e->orden);
    fflush(stdout);

    /* V sobre el semáforo del siguiente proceso. */
    if (indice + 1 < N) {
        op(semid, (unsigned short)(indice + 1), +1);
    }
}

int main(void)
{
    int shmid = shmget(SHM_KEY, sizeof(Estado), IPC_CREAT | 0666);
    int semid = semget(SEM_KEY, N, IPC_CREAT | 0666);
    Estado *e;
    union semun arg;
    unsigned short valores[N] = {1, 0, 0, 0, 0};

    if (shmid == -1 || semid == -1) {
        perror("crear IPC");
        return 1;
    }

    /* S0 empieza disponible; S1..S4 empiezan bloqueados. */
    arg.array = valores;
    if (semctl(semid, 0, SETALL, arg) == -1) {
        perror("semctl SETALL");
        return 1;
    }

    e = shmat(shmid, NULL, 0);
    if (e == (void *)-1) {
        perror("shmat");
        return 1;
    }
    e->posicion = 0;
    e->orden[0] = '\0';

    /*
     * EXACTAMENTE CINCO PROCESOS TOTALES:
     * A = este padre.
     * B,C,D,E = cuatro hijos.
     */
    for (int i = 1; i < N; ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            ejecutar_turno(i, semid, e);
            fflush(stdout);
            shmdt(e);
            _exit(0);
        }
    }

    /* A también participa y consume S0. Luego habilita a B. */
    ejecutar_turno(0, semid, e);

    for (int i = 1; i < N; ++i) {
        wait(NULL);
    }

    printf("[A coordinador] orden final=%s\n", e->orden);

    shmdt(e);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    return 0;
}
