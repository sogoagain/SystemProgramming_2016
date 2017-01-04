#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

#define PRODUCER_FIFO_NAME "/tmp/prod_fifo"
#define CONSUMER_FIFO_NAME "/tmp/cons_fifo"

#define BUFFER_SIZE 128

struct data_to_pass_st {
    pid_t pid;
    char some_data[BUFFER_SIZE-1];
};
