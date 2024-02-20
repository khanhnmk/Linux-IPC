#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>

#define MAX_MSG_SIZE 	256
#define MSG_BUFFER_SIZE (MAX_MSG_SIZE + 10)

int main(int argc, char **argv) {
	
	if(argc <= 1){
        printf("provide a reciepient msgQ name : format </msgq-name>\n");
        return 0;
    }

	mqd_t mqd;
	struct mq_attr attr;
	char buffer[MSG_BUFFER_SIZE];
	memset(buffer, 0, MSG_BUFFER_SIZE);

    mqd = mq_open(argv[1], O_WRONLY | O_CREAT, 0 , 0);
    if(mqd == (mqd_t) -1) {
    	printf("mq_open failed \n");
    	goto return_fail;
    }

    if(mq_getattr(mqd, &attr) == -1) {
    	printf("mq_getattr failed \n");
    	goto return_fail;
    }

    printf("Maximum # of messages on queue: %ld\n", attr.mq_maxmsg);
	printf("Maximum message size: %ld\n", attr.mq_msgsize);
	printf("# of messages currently on queue: %ld\n", attr.mq_curmsgs);

get_buffer_string:
	printf("Enter msg to be sent to reciever %s\n", argv[1]);
    scanf("%s", buffer);

    if (mq_send(mqd, buffer, strlen (buffer) + 1, 0) == -1) {
        printf("Client: Not able to send message to server\n");
        goto return_fail;
    } 

    if(strcmp(buffer, "exit") == 0) {
        printf("msgQ exit\n");
        goto return_ok;
    }

    goto get_buffer_string;

return_ok:
   	mq_close(mqd);
   	mq_unlink(argv[1]);
    return 0;

return_fail:
	mq_close(mqd);
    mq_unlink(argv[1]);
    return -1;
}