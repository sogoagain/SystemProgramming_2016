#include "myshm.h"

static int sem_id;

// 세마포를 초기화한다.
static int set_semvalue(void) {
    union semun sem_union;

    sem_union.val = 1;
    if(semctl(sem_id, 0, SETVAL, sem_union) == -1) return (0);
    return (1);
}

// 세마포를 잠그는 "대기" 연산 P
static int semaphore_p(void) {
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = -1; /* P() */
    sem_b.sem_flg = SEM_UNDO;
    if (semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_p failed\n");
        return(0);
    }

    return(1);
}

// 세마포를 사용할 수 있게 만드는 "신호"연산 V
static int semaphore_v(void) {
    struct sembuf sem_b;

    sem_b.sem_num = 0;
    sem_b.sem_op = 1; /* V() */
    sem_b.sem_flg = SEM_UNDO;

    if(semop(sem_id, &sem_b, 1) == -1) {
        fprintf(stderr, "semaphore_v failed\n");
        return(0);
    }

    return(1);
}

int main(int argc, char **argv) {
    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    int shmid;
    const char* my_NAME = "sogoesagain";

    // 세마포 생성
    sem_id = semget((key_t)5678, 0, 0666 | IPC_CREAT);
    if(sem_id == -1) {
        fprintf(stderr,"Run ipc_producer2 first.\n");
        exit(EXIT_FAILURE);
    }

    // 공유메모리 공간을 만든다.
    shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666 | IPC_CREAT);

    if(shmid == -1) {
        fprintf(stderr, "shmget failed\n");
        exit(EXIT_FAILURE);
    }

    // 공유메모리를 맵핑한다.
    // 공유 메모리 영역을 프로세스 주소 공간에 부착한다.
    shared_memory = shmat(shmid, (void *)0, 0);
    if(shared_memory == (void *)-1) {
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
    }
    shared_stuff = (struct shared_use_st *)shared_memory;

    printf("Waiting Producer...\n");
    if(!semaphore_v()) exit(EXIT_FAILURE);

    // PID와 주소를 읽어들임. 출력.
    if(!semaphore_p()) exit(EXIT_FAILURE);
    printf("PRODUCER: %d, ADDR: %s\n", shared_stuff->pid, shared_stuff->some_text);
    shared_stuff->pid = getpid();
    strcpy(shared_stuff->some_text, my_NAME);
    if(!semaphore_v()) exit(EXIT_FAILURE);

    // 공유메모리를 떼어낸다.
    if(shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
