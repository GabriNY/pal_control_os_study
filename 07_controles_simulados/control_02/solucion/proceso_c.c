/* CONTROL 02 - C recibe el identificador qid por argv y lee MQ tipo 5. */
#include <stdio.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <unistd.h>

struct mensaje {
    long type;
    char text[128];
};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Uso: %s qid\n", argv[0]);
        return 1;
    }

    int qid = atoi(argv[1]);
    struct mensaje msg;

    if (msgrcv(qid, &msg, sizeof(msg.text), 5, 0) == -1) {
        perror("C msgrcv");
        return 1;
    }

    printf("[C PID=%d] MQ type=%ld: %s\n", getpid(), msg.type, msg.text);
    return 0;
}
