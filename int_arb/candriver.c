/**
 *int_arb/candriver.c
 *2-15-09
 *jrm
 *Receive a command(com), a pointer to array of chars(*message_array[])
 *and a pointer to a message id(*msg_id).
 *return 0=success, 1=can down, 2=buffer error, 3=can errors, 4=buffer empty, 5=
 *6=can busy, 7=invalid command arg
 *command summary:0=initialize, 1=receive, 2=send, 3=filter management(unfinished)
 */
#include<stdio.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include"include/can.h"

//FIXME if the data field is 8 bytes, as defined in canmsg.h, will incomplete
//message bytes be left in the can buffer?
int Driver(unsigned char com, char *message_array[], unsigned long *msg_id)
{
	int fd=0, ret=0, i=0;
	char specialfile[]="/dev/can0";
	struct canmsg_t message;

	switch (com) {
		case 0: //ini
			fd = open(specialfile, O_RDWR);
			if (fd<0) {
				printf("Error opening %s\n", specialfile);
				return 1;
			}
			return 0;

		case 1: //receive
			ret = read(fd, &message, sizeof(struct canmsg_t));
			if (ret<0)
				return 3;
			else {
				*msg_id = message.id;
				for (i=0; i>message.length; i++) {
					*message_array[i] = message.data[i];
				}
			}
			return 0;

		case 2: //send
			message.id = *msg_id;
			for (i=0; i==5; i++) {
				message.data[i] = *message_array[i];
			}
			ret = write(fd, &message, sizeof(struct canmsg_t));
			if (ret<0)
				return 3;
			else
				return 0;
	}
}
