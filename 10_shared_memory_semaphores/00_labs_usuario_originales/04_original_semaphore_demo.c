#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 0x1111

union semun
{
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

union semun u;

struct sembuf p = {0, -1, 0};
struct sembuf v = {0, +1, 0};

void semP(int id)
{
    if (semop(id, &p, 1) < 0)
    {
        perror("semop p");
        exit(13);
    }
}

void semV(int id)
{
    if (semop(id, &v, 1) < 0)
    {
        perror("semop p");
        exit(14);
    }
}

int getSemCreate()
{
        int i = semget(KEY, 1, 0666 | IPC_CREAT);
        if (i < 0)
        {
            perror("semget");
            exit(11);
        }
    return i;
}

int getSem()
{
    int i = semget(KEY, 1, 0 );
    if (i < 0)
    {
            perror("semget");
            exit(11);
    }
    return i;
}


void controlSem(int id, int numRecursos)
{
    //union semun u;
    u.val = numRecursos;
    if (semctl(id, 0, SETVAL, u) < 0)
    {
        perror("semctl");
        exit(12);
    }
}

int main(int argc, char *argv[]) 
{
    char c;
    int id ;
    
    
    

        if ( argv[1][0]=='c'){
            id = getSemCreate();
            controlSem(id, 1);
        }
        if ( argv[1][0]=='g'){
            id = getSem();
        }
        
        printf("sem:%d\n",id);

        char *s = argv[2];
        int l = strlen(s);
        for (int i = 0; i < l; ++i)
        {
            semP(id);
            putchar(s[i]);
            fflush(stdout);
            sleep(rand() % 2);
            putchar(s[i]);
            fflush(stdout);
                if (argv[2][0] == 'a')
                    scanf(" %c", &c);
            semV(id);
            sleep(rand() % 2);
        }

}