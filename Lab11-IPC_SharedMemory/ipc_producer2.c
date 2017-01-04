#include "myshm.h"

#define INPUT_LENGTH 20

static int sem_id;

// 세마포를 초기화한다.
static int set_semvalue(void) {
    union semun sem_union;

    sem_union.val = 1;
    if(semctl(sem_id, 0, SETVAL, sem_union) == -1) return (0);
    return (1);
}

// 세마포 식별자를 삭제한다.
static void del_semvalue(void) {
    union semun sem_union;

    if(semctl(sem_id, 0, IPC_RMID, sem_union) == -1)
        fprintf(stderr, "Failed to delete semaphore\n");
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
    int running = 1;
    void *shared_memory = (void *)0;
    struct shared_use_st *shared_stuff;
    int shmid;
    char str_input[INPUT_LENGTH];
    const char* my_Addr = "sogoesagain.github.io";

    // 세마포 생성
    sem_id = semget((key_t)5678, 1, 0666 | IPC_CREAT);
    if(!set_semvalue()) {
        fprintf(stderr, "Failed to initialize semaphore\n");
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

    // 공유 메모리 영역에 shared_stuff를 배정하고 written_by_you를 0으로 설정한다.
    // 이러면 producer  프로그램이 공유 메모리 영역에 자신의 자료를 기록할 수 있게 된다.
    // 루프로 진입해서는 생산자가 자료를 기록하길 기다렸다가 기록된 자료를 출력한다.
    shared_stuff = (struct shared_use_st *)shared_memory;

    if(!semaphore_p()) exit(EXIT_FAILURE);
    printf("Waiting Consumer...\n");

    if(!semaphore_p()) exit(EXIT_FAILURE);
    printf("Enter 'start' to start.\n");
 
    do {
        fgets(str_input, INPUT_LENGTH, stdin);
    } while(strncmp(str_input, "start", 5) != 0);

    // 전달할 데이터 입력 (pid와 주소) 
    shared_stuff->pid = getpid();
    strcpy(shared_stuff->some_text, my_Addr);
    if(!semaphore_v()) exit(EXIT_FAILURE);
    
    // CONSUMER의 PID와 이름 읽어들임.
    if(!semaphore_p()) exit(EXIT_FAILURE);
        printf("PRODUCER: %d, CONSUMER: %d, ADDR: %s, NAME: %s\n", getpid(), shared_stuff->pid, my_Addr, shared_stuff->some_text);
    if(!semaphore_v()) exit(EXIT_FAILURE);
    
    // 공유메모리를 떼어내고 삭제한다.
    if(shmdt(shared_memory) == -1) {
        fprintf(stderr, "shmdt failed\n");
        exit(EXIT_FAILURE);
    }

    if(shmctl(shmid, IPC_RMID, 0) == -1) {
        fprintf(stderr, "shmctl(IPC_RMID) failed\n");
        exit(EXIT_FAILURE);
    }

    del_semvalue();
    exit(EXIT_SUCCESS);
}
