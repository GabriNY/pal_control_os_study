#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 0x5201
#define SEM_KEY 0x6201

typedef struct {
    int contador;
    int terminado;
    char mensaje[128];
} DatosCompartidos;

union semun { int val; struct semid_ds *buf; unsigned short *array; };
static struct sembuf P = {0, -1, 0};
static struct sembuf V = {0, +1, 0};

static void down(int id) { if (semop(id, &P, 1) == -1) { perror("semop P"); exit(1); } }
static void up(int id)   { if (semop(id, &V, 1) == -1) { perror("semop V"); exit(1); } }

int main(void)
{
    int shmid = shmget(SHM_KEY, sizeof(DatosCompartidos), IPC_CREAT | 0666);
    int semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    union semun arg;
    DatosCompartidos *d;

    if (shmid == -1 || semid == -1) { perror("crear IPC"); return 1; }
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) { perror("semctl"); return 1; }

    d = shmat(shmid, NULL, 0);
    if (d == (void *)-1) { perror("shmat"); return 1; }

    down(semid);
    d->contador = 0;
    d->terminado = 0;
    snprintf(d->mensaje, sizeof(d->mensaje), "Memoria inicializada por PID %d", getpid());
    up(semid);

    printf("[coordinador] SHMID=%d SEMID=%d. Ejecute ./cliente\n", shmid, semid);

    while (1) {
        int listo;
        down(semid);
        listo = d->terminado;
        up(semid);
        if (listo) break;
        sleep(rand() % 2);
    }

    down(semid);
    printf("[coordinador] contador=%d mensaje='%s'\n", d->contador, d->mensaje);
    up(semid);

    shmdt(d);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    return 0;
}
