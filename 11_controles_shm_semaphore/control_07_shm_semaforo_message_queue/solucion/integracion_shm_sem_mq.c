#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>

#define SHM_KEY 0x5207
#define SEM_KEY 0x6207
#define MQ_KEY  0x7207
#define HIJOS 4

typedef struct { long mtype; pid_t pid; int valor; } Mensaje;
union semun { int val; struct semid_ds *buf; unsigned short *array; };

static void sem_op(int id, short delta) {
    struct sembuf sb = {0, delta, 0};
    if (semop(id, &sb, 1) == -1) { perror("semop"); _exit(1); }
}

int main(void)
{
    int shmid = shmget(SHM_KEY, sizeof(int), IPC_CREAT | 0666);
    int semid = semget(SEM_KEY, 1, IPC_CREAT | 0666);
    int msqid = msgget(MQ_KEY, IPC_CREAT | 0666);
    union semun arg;
    int *contador;

    if (shmid == -1 || semid == -1 || msqid == -1) { perror("crear IPC"); return 1; }
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) { perror("SETVAL"); return 1; }
    contador = shmat(shmid, NULL, 0);
    if (contador == (void *)-1) { perror("shmat"); return 1; }
    *contador = 0;

    for (int i = 0; i < HIJOS; ++i) {
        pid_t pid = fork();
        if (pid == -1) { perror("fork"); return 1; }
        if (pid == 0) {
            Mensaje m = {.mtype = 1, .pid = getpid(), .valor = 0};

            sem_op(semid, -1);
            ++(*contador);
            m.valor = *contador;
            sem_op(semid, +1);

            if (msgsnd(msqid, &m, sizeof(m) - sizeof(long), 0) == -1) {
                perror("msgsnd");
            }
            fflush(stdout);
            shmdt(contador);
            _exit(0);
        }
    }

    for (int i = 0; i < HIJOS; ++i) {
        Mensaje m;
        if (msgrcv(msqid, &m, sizeof(m) - sizeof(long), 1, 0) == -1) {
            perror("msgrcv");
            return 1;
        }
        printf("[A] mensaje de PID=%d: contador observado=%d\n", (int)m.pid, m.valor);
    }

    for (int i = 0; i < HIJOS; ++i) wait(NULL);
    printf("[A] contador final en SHM=%d\n", *contador);

    shmdt(contador);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}
