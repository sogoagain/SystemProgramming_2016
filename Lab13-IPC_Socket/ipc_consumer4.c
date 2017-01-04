#include "mysocket.h"

int main(int agrc, char **argv) {
    int sockfd;
    int len;
    struct sockaddr_un address;
    int result;
    struct my_msg_st some_data;
    const char *my_NAME = "sogoesagain";

    // 소비자쪽 소켓을 만든다.
    sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

    // 공급자쪽 소켓 이름을 지정한다.
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, "producer_socket");
    len = sizeof(address);

    // 소비자 소켓을 공급자쪽 소켓에 연결한다.
    result = connect(sockfd, (struct sockaddr *)&address, len);

    if(result == -1) {
        perror("oops: Consumer");
        exit(1);
    }

    printf("Waiting Producer...\n");

    // sockfd를 통해서 자료를 읽고 쓴다.
    read(sockfd, (void *)&some_data, sizeof(some_data));
    printf("PRODUCER: %d, ADDR: %s\n", some_data.pid, some_data.some_text);
    
    some_data.pid = getpid();
    strcpy(some_data.some_text, my_NAME);
    write(sockfd, (void *)&some_data, sizeof(some_data));

    close(sockfd);
    exit(0);
}
