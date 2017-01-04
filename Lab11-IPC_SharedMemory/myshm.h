#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define TEXT_SZ 2048

struct shared_use_st {
    pid_t pid;
    char some_text[TEXT_SZ];
};

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short int *array;
};
