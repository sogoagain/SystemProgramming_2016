#include "mysocket.h"

#define INPUT_LENGTH 20

int main(int argc, char **argv) {
    int producer_sockfd, consumer_sockfd;
    int producer_len, consumer_len;
    struct sockaddr_un producer_addr;
    struct sockaddr_un consumer_addr;
    struct my_msg_st some_data;
    const char *my_Addr = "sogoagain.github.io";
    char str_input[INPUT_LENGTH];

    // 기존의 명명된 소켓을 삭제하고, 서버를 위한 이름없는 소켓을 생성한다.
    unlink("producer_socket");
    producer_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    // 소켓에 이름 부여
    producer_addr.sun_family = AF_UNIX;
    strcpy(producer_addr.sun_path, "producer_socket");
    producer_len = sizeof(producer_addr);
    bind(producer_sockfd, (struct sockaddr *)&producer_addr, producer_len);

    // 연결 대기열을 생성하고 클라이언트의 요청을 기다린다.
    listen(producer_sockfd, 1);

        printf("Waiting Consumer...\n");

    // 연결을 수락한다.
    consumer_len = sizeof(consumer_addr);
    consumer_sockfd = accept(producer_sockfd, (struct sockaddr *)&consumer_addr, &consumer_len);

    printf("Enter 'start' to start.\n"); 
    do {
        fgets(str_input, INPUT_LENGTH, stdin);
    } while(strncmp(str_input, "start", 5) != 0);

    // consumer_sockfd를 이용해서 소비자와 자료를 주고받는다.
    some_data.pid = getpid();
    strcpy(some_data.some_text, my_Addr);
    write(consumer_sockfd, (void *)&some_data, sizeof(some_data));

    read(consumer_sockfd, (void *)&some_data, sizeof(some_data));
    printf("PRODUCER: %d, CONSUMER: %d, ADDR: %s, NAME: %s\n", getpid(), some_data.pid, my_Addr, some_data.some_text);

    close(consumer_sockfd);
    unlink("producer_socket");
}
