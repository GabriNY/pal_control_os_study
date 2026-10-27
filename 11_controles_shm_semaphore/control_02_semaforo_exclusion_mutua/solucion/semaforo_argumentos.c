/*
 * Basado en 11_Semaphore.c.
 * Dos o más instancias pueden imprimir cadenas sin mezclar cada pareja
 * de caracteres porque cada iteración entra a una sección crítica P/V.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define SEM_KEY 0x1111

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

static struct sembuf operacion_p = {0, -1, 0};
static struct sembuf operacion_v = {0, +1, 0};

static void semP(int semid)
{
    if (semop(semid, &operacion_p, 1) == -1) {
        perror("semop P");
        exit(13);
    }
}

static void semV(int semid)
{
    if (semop(semid, &operacion_v, 1) == -1) {
        perror("semop V");
        exit(14);
    }
}

static int crear_semaforo(void)
{
    int semid = semget(SEM_KEY, 1, 0666 | IPC_CREAT);
    union semun arg;

    if (semid == -1) {
        perror("semget crear");
        exit(11);
    }

    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl SETVAL");
        exit(12);
    }
    return semid;
}

static int obtener_semaforo(void)
{
    int semid = semget(SEM_KEY, 1, 0);
    if (semid == -1) {
        perror("semget obtener");
        exit(11);
    }
    return semid;
}

int main(int argc, char *argv[])
{
    int semid;
    const char *texto;

    if (argc != 3) {
        fprintf(stderr, "Uso: %s c|g TEXTO\n", argv[0]);
        fprintf(stderr, "  c = crea/inicializa semaforo; g = obtiene uno existente\n");
        return 1;
    }

    semid = (argv[1][0] == 'c') ? crear_semaforo() : obtener_semaforo();
    texto = argv[2];

    printf("[PID=%d] semid=%d texto=%s\n", getpid(), semid, texto);

    for (size_t i = 0; i < strlen(texto); ++i) {
        /* P: si el valor está en 0, este proceso duerme hasta que otro haga V. */
        semP(semid);

        /* Sección crítica: se imprimen dos copias del mismo carácter juntas. */
        putchar(texto[i]);
        fflush(stdout);
        sleep(rand() % 2);
        putchar(texto[i]);
        fflush(stdout);

        /* V: libera el recurso para otro proceso. */
        semV(semid);
        sleep(rand() % 2);
    }

    putchar('\n');
    return 0;
}
