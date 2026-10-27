#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define SHM_KEY 0x5201
#define SEM_KEY 0x6201

typedef struct { int contador; int terminado; char mensaje[128]; } DatosCompartidos;
static struct sembuf P = {0, -1, 0};
static struct sembuf V = {0, +1, 0};
static void down(int id) { if (semop(id, &P, 1) == -1) { perror("semop P"); exit(1); } }
static void up(int id)   { if (semop(id, &V, 1) == -1) { perror("semop V"); exit(1); } }

int main(void)
{
    int shmid = shmget(SHM_KEY, sizeof(DatosCompartidos), 0666);
    int semid = semget(SEM_KEY, 1, 0);
    DatosCompartidos *d;

    if (shmid == -1 || semid == -1) { perror("obtener IPC"); return 1; }
    d = shmat(shmid, NULL, 0);
    if (d == (void *)-1) { perror("shmat"); return 1; }

    down(semid);
    ++d->contador;
    snprintf(d->mensaje, sizeof(d->mensaje), "Cliente PID %d modifico la memoria", getpid());
    d->terminado = 1;
    up(semid);

    printf("[cliente] modificación terminada.\n");
    shmdt(d);
    return 0;
}
