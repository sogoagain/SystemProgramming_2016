#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_THREAD 10
#define MAX_CALL 20

void *test_func(void *arg) {
    int cnt_call = 0;
    struct tm *ptr_tm;
    time_t timevar;
    char buf[16];
    int id = *(int*)arg;
    int r = 0;
    
    
    for(cnt_call = 0; cnt_call < MAX_CALL; cnt_call++) {
        r = rand()%10;
        sleep(r);
        (void)time(&timevar);
        ptr_tm = localtime(&timevar);
        strftime(buf, 16, "%H:%M:%S", ptr_tm);
        printf("ID: %2d, TIME: %s, COUNT: %d\n", id, buf, cnt_call+1);
    }
    
    pthread_exit(NULL);
}

int main() {
    pthread_t p_thread[NUM_OF_THREAD];
    int thr_id[NUM_OF_THREAD];
    void *thread_result;
    int res;
    int status;
    int i = 0;
    
    printf("sogoesagain.github.io\n");
    srand(time(NULL));
    
    for(int i = 0; i < NUM_OF_THREAD; i++) {
        thr_id[i] = i+1;
        res = pthread_create(&(p_thread[i]), NULL, test_func, (void*)&thr_id[i]);
        if (res != 0) {
            perror("thread create error: ");
            exit(EXIT_FAILURE);
        }
    }
    
    for(i = NUM_OF_THREAD - 1; i >= 0; i--) {
        res = pthread_join(p_thread[i], &thread_result);
        if(res != 0) {
            perror("pthread_join failed");
        }
    }
    
    
    printf("Finished (thread)\n");
    
    exit(EXIT_SUCCESS);
}

