#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_CALL 20

void test_func(void) {
    static int cnt_call = 0;
    struct tm *ptr_tm;
    time_t timevar;
    char buf[16];
    
    (void)time(&timevar);
    ptr_tm = localtime(&timevar);
    strftime(buf, 16, "%H:%M:%S", ptr_tm);
    
    printf("PID: %2d, TIME: %s, COUNT: %d\n", getpid(), buf, cnt_call+1);
    
    cnt_call++;
}

int main(int argc, char** argv) {
    int i = 0;
    int r = 0;
    
    srand(getpid());
    
    for(i = 0; i < MAX_CALL; i++) {
        r = rand()%10;
        sleep(r);
        test_func();
    }
    
    exit(0);
}

