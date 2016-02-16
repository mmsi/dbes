/**
 *int_arb/candriver.c
 */

/**
 *(C) 2010 by Joel R. Morgan <jrcowboy79@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor Boston, MA 02110-1301,  USA
 */
 
/**
 *Receive a command(com), a pointer to array of chars(*message_array[])
 *and a pointer to a message id(*msg_id).
 *return 0=success, 1=can down, 2=queue full, 3=can errors, 4=queue empty, 5=
 *6=can busy, 7=invalid command arg FIXME put return values in defines
 *command summary:0=initialize, 1=receive, 2=send, 3=filter management(unfinished)
 */
#include<stdio.h>
#include<sys/ioctl.h>
#include<fcntl.h>
#include<unistd.h>
#include"include/can.h"
#include"include/candriver.h"

#define CAN_WAIT_USEC 5

int can_fd_wait(int, int);

//The read() gets the full message and puts it in a canmsg_t
//any unneedded bytes are just abandoned
int Driver(unsigned char com, unsigned char *msg_data, unsigned long *msg_id)
{
	static int fd=0;
	int ret=0, i=0;
	char specialfile[]="/dev/can0";
	struct canmsg_t message;

	switch (com) {
		case CAN_INI: //ini
			fd = open(specialfile, O_RDWR|O_SYNC);
			if (fd<0) {
				printf("Error opening %s\n", specialfile);
				return -1;
			}
			return 0;

		case CAN_RX: //receive
			ret = can_fd_wait(fd, CAN_WAIT_USEC);
			*msg_id  = 0;
			//printf("ret = %i\n", ret);
			if (ret != 0) {
				ret = read(fd, &message, sizeof(struct canmsg_t));
				//printf("CAN bytes: %i\n", ret);
				//printf("message ID = %i\n", message.id);
				if (ret<0)
					return -1;
				else {
					*msg_id = message.id;
					for (i=0; i<7; i++) {
						*(msg_data + i) = message.data[i];
					}
				}
			}
			return 0;

		case CAN_TX: //send XXX FIXME check if successful
		    message.flags = 0;
			message.id = *msg_id;
			message.length = 8;
			for (i=0; i<7; i++) {
				message.data[i] = *(msg_data + i);
			}
			ret = write(fd, &message, sizeof(struct canmsg_t));
			if (ret<0)
				return -1;
			else
				return 0;
		
		case CAN_CLOSE:
		    close(fd);
		    return 0;
		    
		default:
			return -1;
	}
}

int can_fd_wait(int fd, int wait_usec)
{
	struct timeval timeout;
	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd, &set);
	timeout.tv_sec = 0;
	timeout.tv_usec = wait_usec;
	return (select(FD_SETSIZE, &set, NULL, NULL, &timeout));
}
