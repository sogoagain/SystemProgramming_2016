#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_TEXT 512

struct my_msg_st {
    pid_t pid;
    char some_text[MAX_TEXT];
};
