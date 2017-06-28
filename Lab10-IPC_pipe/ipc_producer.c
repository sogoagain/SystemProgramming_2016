#include "myfifo.h"

int main(int argc, char **argv) {
    int producer_fifo_fd, consumer_fifo_fd;
    struct data_to_pass_st my_data;
    int read_res;
    const char *my_Addr = "sogoagain.github.io";
    char str_input[BUFFER_SIZE];

    // FIFO 파일을 생성한다.
    mkfifo(PRODUCER_FIFO_NAME, 0777);
    mkfifo(CONSUMER_FIFO_NAME, 0777);

    printf("Waiting Consumer ...\n");
   
    // ipc_producer 입장에서 전달 받는 통로
    // ipc_producer는 자신의 FIFO를 읽기 전용 차단 모드로 연다.
    // 이 open 호출은 ipc_consumer가 같은 FIFO를 쓰기 모드로 열 때까지 차단된다.
    producer_fifo_fd = open(PRODUCER_FIFO_NAME, O_RDONLY);
    if(producer_fifo_fd == -1) {
        fprintf(stderr, "Producer fifo failure\n");
        exit(EXIT_FAILURE);
    }

    // ipc_consumer 입장에서 전달 받는 통로
    // ipc_producer은 이 fifo로 데이터를 전송한다.
    consumer_fifo_fd = open(CONSUMER_FIFO_NAME, O_WRONLY);
    if(consumer_fifo_fd == -1) {
        fprintf(stderr, "Consumer fifo failure\n");
        exit(EXIT_FAILURE);
    }

    printf("Enter 'start' to start.\n");
 
    do {
        fgets(str_input, BUFFER_SIZE, stdin);
    } while(strncmp(str_input, "start", 5) != 0);

    // ipc_consumer에게 데이터 전송
    // ipc_producer의 pid와 주소 전송
    my_data.pid = getpid();
    strcpy(my_data.some_data, my_Addr); 
    write(consumer_fifo_fd, &my_data, sizeof(my_data));

    // ipc_consumer로부터 전달받은 데이터 읽기
    // ipc_consumer의 pid와 이름 전달받음.
    read_res = read(producer_fifo_fd, &my_data, sizeof(my_data));
    if(read_res > 0) {
        printf("PRODUCER: %d, CONSUMER: %d, ADDR: %s, NAME: %s\n",getpid(), my_data.pid, my_Addr, my_data.some_data);
    }
    
    close(consumer_fifo_fd);
    close(producer_fifo_fd);
    unlink(PRODUCER_FIFO_NAME);
    exit(EXIT_SUCCESS);
}
