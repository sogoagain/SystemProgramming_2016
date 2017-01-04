#include "myfifo.h"

int main(int argc, char** argv) {
    int producer_fifo_fd, consumer_fifo_fd;
    struct data_to_pass_st my_data;
    const char *my_NAME = "sogoesagain";
    int read_res = 0;

    producer_fifo_fd = open(PRODUCER_FIFO_NAME, O_WRONLY);
    if(producer_fifo_fd == -1) {
        printf("Run ipc_producer first.\n");
        fprintf(stderr, "Producer fifo failure\n");
        exit(EXIT_FAILURE);
    }

    consumer_fifo_fd = open(CONSUMER_FIFO_NAME, O_RDONLY);
    if(consumer_fifo_fd == -1) {
        fprintf(stderr, "Consumer fifo failure\n");
        exit(EXIT_FAILURE);
    }
    
    printf("Waiting Producer...\n");

    read_res = read(consumer_fifo_fd, &my_data, sizeof(my_data));
    printf("PRODUCER: %d, ADDR: %s\n", my_data.pid, my_data.some_data);

    my_data.pid = getpid();
    strcpy(my_data.some_data, my_NAME);
    
    write(producer_fifo_fd, &my_data, sizeof(my_data));
    close(producer_fifo_fd);

    close(consumer_fifo_fd);
    unlink(CONSUMER_FIFO_NAME);
    
    exit(EXIT_SUCCESS);
}
