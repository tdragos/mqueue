#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <string.h>
#include <stdlib.h>

#define BUF_SIZE 8192

#define server_name "/server_queue"

int main(void) {

    mqd_t s_queue;
    mqd_t c_queue;
    int seq_number = 0;
    int status;
    int prio = 1;
    char buf[BUF_SIZE];
    char to_send[10];

    //open server's message queue
    s_queue = mq_open(server_name, O_CREAT | O_RDWR, 0666, NULL);
    if (s_queue == (mqd_t)-1) {
        perror("server_queue");
        exit(1);
    }

    while(1) {

        //clear the buffer and wait for client's message
        memset(buf, '\0', BUF_SIZE);
        status = mq_receive(s_queue, buf, BUF_SIZE, &prio);
        if (status == -1) {
            perror("mq_recv");
            exit(1);
        }

        //print client's name, if it is /STOP we exit
        printf("%s\n", buf);
        if (!strcmp(buf, "/STOP"))
            break;

        //open client's message queue
        c_queue = mq_open(buf, O_CREAT | O_RDWR, 0666, NULL);
        if (c_queue == (mqd_t)-1) {
            perror("client queue");
            exit(1);
        }

        //write the message and sent it
        sprintf(to_send, "%d", seq_number++);
        status = mq_send(c_queue, to_send, strlen(to_send), prio);
        if (status == -1) {
            perror("mq_send");
            exit(1);
        }
    }

    //close and unlink
    status = mq_close(s_queue);
    if (status == -1) {
        perror("server_close");
        exit(1);
    }

    status = mq_unlink(server_name);
    if (status == -1) {
        perror("server_unlink");
        exit(1);
    }

    return 0;
}
