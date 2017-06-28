#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define MAX_CHILD 10

int run_process = 0;

void zombie_handler() {
    int status;
    int spid;
    spid = wait(&status);
    printf("종료된 PID : %d\n", spid);
    
    run_process--;
    printf("실행중인 subproc: %d개\n", run_process);
}

int main(int argc, char** argv) {
    int i  = 0;
    
    printf("sogoagain.github.io\n");
    int pid;
    
    signal(SIGCHLD, (void *)zombie_handler);
    
    for(i = 0; i < MAX_CHILD; i++) {
        pid = fork();
        if (pid < 0) {
            fprintf(stderr, "Fork Error");
        }
        if (pid == 0) {
            if(execl("./subproc", "./subproc", NULL) == -1)
                fprintf(stderr, "subproc Exec Error\n");
            exit(0);
        }
        if (pid > 0) {
            run_process++;
        }
    }
    
    while(run_process != 0) {
    }
    
    printf("Finished (process)\n");
    
    
    return 0;
}
