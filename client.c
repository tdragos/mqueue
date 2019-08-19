#include <stdio.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <mqueue.h>

#define BUF_SIZE 8192

int main(int argc, char **argv) {

    int status;
    mqd_t s_queue;
    mqd_t c_queue;
    int prio = 1;
    char buf[BUF_SIZE];
    const char server_name[] = "/server_queue";

    //arg[1] must be the client's message queue name
    if (argc < 2) {
        printf("Usage is ./client /[client_name]\n");
        exit(1);
    }

    const char *client_name = argv[1];

    //open the server message queue
    s_queue = mq_open(server_name, O_CREAT | O_RDWR, 0666, NULL);
    if (s_queue == (mqd_t)-1) {
        perror("server_queue");
        exit(1);
    }

    //send client's name to the server
    status = mq_send(s_queue, client_name, strlen(client_name), prio);
    if (status == -1) {
        perror("send_name");
        exit(1);
    }

    //if the client's name is /STOP we exit
    if (!strcmp(client_name, "/STOP"))
        return 0;

    //open client's message queue
    c_queue = mq_open(client_name, O_CREAT | O_RDWR, 0666, NULL);
    if (c_queue == (mqd_t)-1) {
        perror("client_queue");
        exit(1);
    }

    //wait for the server's answer
    status = mq_receive(c_queue, buf, BUF_SIZE, &prio);
    if (status == -1) {
        perror("receive");
        exit(1);
    }

    printf("I got the message: %s\n", buf);

    //close and unlink
    status = mq_close(c_queue);
    if (status == -1) {
        perror("client_close");
        exit(1);
    }

    status = mq_unlink(client_name);
    if (status == -1) {
        perror("client_unlink");
        exit(1);
    }

    return 0;
}
