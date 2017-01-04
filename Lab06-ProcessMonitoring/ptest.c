#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

/*
 Signal 핸들러 함수
*/
void ouch(int sig) {
    printf("OUCH! - I got signal %d\n", sig);
}

void hand_sigterm(int sig) {
    exit(0);
}

int main(int argc, char* argv[]) {
    
    /*
     start_time: ptest 최초 구동 시각
     last_time: ptest 현재 구동 시각
     둘의 차이가 ptest 총 구동 시간
     */
    time_t start_time, last_time;
    double diff;

    /* sigaction을 이용해 SIGALRM 신호를 처리한다. */
    struct sigaction act;

    act.sa_handler = ouch;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGALRM, &act, 0);
    signal(SIGTERM, hand_sigterm);

    start_time = time((time_t*)0);

    while(1) {
        last_time = time((time_t*)0);
        printf("ptest run-time %gs\n",difftime(last_time, start_time));
        fflush(stdout);
        sleep(2);
    }
    exit(0);
}
