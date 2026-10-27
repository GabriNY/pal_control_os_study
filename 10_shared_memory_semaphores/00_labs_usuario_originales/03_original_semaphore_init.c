#include <stdio.h> 
#include <stdlib.h> 
#include <errno.h> 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/sem.h>

union semun {
    int val;               /* used for SETVAL only */
    struct semid_ds *buf;  /* used for IPC_STAT and IPC_SET */
    ushort *array;         /* used for GETALL and SETALL */
};

int main(void)
{
    key_t key;
    int semid;
    union semun arg;
    key = 0x4444;
    /*
    if ((key = ftok("seminit.c", 'J')) == -1) 
    {   
        perror("ftok");
        exit(1); 
    }
    */

/* create a semaphore set with 1 semaphore: */
    if ((semid = semget(key, 1, 0666 | IPC_CREAT)) == -1) 
    {
        perror("semget");
        exit(1); 
    }
/* initialize semaphore #0 to 1: */ 
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) 
    {
        perror("semctl");
        exit(1); 
    }
return 0; 
    
}