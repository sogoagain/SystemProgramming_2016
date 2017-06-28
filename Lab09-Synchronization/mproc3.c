#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_OF_THREAD 10
#define MAX_CALL 20

int base_count = 1; // 쓰레드간 공유되는 자원
int check_change = 0;   // 값을 변경한 쓰레드의 갯수 (10이면 모두 변경 완료)
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // 쓰레드 초기화

// 쓰레드 함수
void *test_func(void *arg) {
    int cnt_call = 0;   // 함수 호출 횟수
    struct tm *ptr_tm;
    time_t timevar;
    char buf[16];
    int id = *(int*)arg;
    int r = 0;
    int time_update = 1;
    
    while(cnt_call < MAX_CALL) {
        if(time_update) {
            r = rand()%10;
            sleep(r);
            (void)time(&timevar);
            ptr_tm = localtime(&timevar);
            strftime(buf, 16, "%H:%M:%S", ptr_tm);
        }
         
        if (cnt_call < base_count) {
            pthread_mutex_lock(&mutex); // 잠금을 생성한다.
            printf("ID: %2d, TIME: %s, COUNT: %d\n", id, buf, cnt_call+1);
            
            check_change++;
            if(check_change == NUM_OF_THREAD) {
                check_change = 0;
                base_count++;
            }
            pthread_mutex_unlock(&mutex);   // 잠금을 해제한다.
            
            cnt_call++;
            time_update = 1;
        }
        else {
            time_update = 0;
        }
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
    
    printf("sogoagain.github.io\n");
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
    pthread_mutex_destroy(&mutex);
    printf("Finished Successfully\n");
    
    exit(EXIT_SUCCESS);
}

