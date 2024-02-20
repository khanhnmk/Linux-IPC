#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>
#include <errno.h>
#include <time.h>

#define MAX_MESSAGES 10
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE     (MAX_MSG_SIZE + 10)
#define QUEUE_PERMISSIONS   0660


int 
main(int argc, char **argv){

    fd_set readfds;
    struct timespec tm;
    clock_gettime(CLOCK_REALTIME, &tm);
    printf("tm.tv_sec: %ld \n", tm.tv_sec);
	tm.tv_sec += 20;  // Set for 20 seconds
    char buffer[MSG_BUFFER_SIZE];
    int mqd = 0;

    if(argc <= 1){
        printf("provide a reciepient msgQ name : format </msgq-name>\n");
        return 0;
    }

    /*To set msgQ attributes*/
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = MAX_MESSAGES;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    if ((mqd  = mq_open (argv[1], O_RDONLY | O_CREAT, QUEUE_PERMISSIONS, &attr)) == -1) {
        printf ("Client: mq_open failed, errno = %d", errno);
        goto return_fail;
    }

    /**** While ****/
    // while(1){
    //     // FD_ZERO(&readfds);
    //     // FD_SET( mqd, &readfds);
    //     // printf("Reciever blocked on select()....\n");
    //     // select(mqd + 1, &readfds, NULL, NULL, NULL);
    //     // if(FD_ISSET(mqd, &readfds)){
    //         printf("Msg recvd msgQ %s\n", argv[1]);
    //         memset(buffer, 0, MSG_BUFFER_SIZE);
    //         if (mq_receive (mqd, buffer, MSG_BUFFER_SIZE, NULL) == -1) {
    //             printf ("mq_receive error, errno = %d\n", errno);
    //             goto return_fail;
    //         }
    //         printf("Msg recieved from Queue = %s\n", buffer);
    //         if(strcmp(buffer, "exit") == 0) {
    //         	printf("msgQ exit\n");
    //         	goto return_ok;
    //         }
    //     // }
    // }
    /**** end. ****/

    /**** Timeout ****/
    printf("Msg recvd msgQ %s\n", argv[1]); 
    memset(buffer, 0, MSG_BUFFER_SIZE);
    if (mq_timedreceive(mqd, buffer, MSG_BUFFER_SIZE, NULL, &tm) == -1) {
        printf ("mq_receive error, errno = %d\n", errno);
        goto return_fail;
    }
    printf("Msg recieved from Queue = %s\n", buffer);
    if(strcmp(buffer, "exit") == 0) {
    	printf("msgQ exit\n");
    	goto return_ok;
    }
    /**** end. ****/

return_ok:
   	mq_close(mqd);
   	mq_unlink(argv[1]);
    return 0;

return_fail:
	mq_close(mqd);
    mq_unlink(argv[1]);
    return -1;
}