/**
 *intarb/slave.c
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

#define _GNU_SOURCE
#include<unistd.h>
#include<time.h>
#include"../include/control_str.h"
#include"include/intarb.h"
#include"include/candriver.h"

/* ID mapping delay (seconds) */
#define UPDATE 180

int Slave(struct cnt_template_t *control, struct status_t *local_status)
{
	int ret;
	unsigned char s_message_array[MSG_LENGTH];
	unsigned long s_msg_id;

	/* ID mapping delays */
	static int mapped = 0;
	static time_t updatetv;

	if ((difftime(time(NULL),updatetv))>UPDATE)
		mapped = 0;
	
	/*normal slave operations*/
	do {
		ret = Driver(CAN_RX, s_message_array, &s_msg_id);
		if (ret != 0)
			return -1;
		switch (s_msg_id) {
			case CONTROL:
				control->function = s_message_array[0];
				//control->id = s_message_array[1];
				control->dest = s_message_array[3];
				control->dest = ((control->dest << 8) + s_message_array[2]);
				control->rate = s_message_array[4];
				break;

			case ZERO:
				control->function |= 0x20;
				break;

			case ID_PING:
				if (mapped == 0) {
					s_message_array[0] = local_id; //XXX does id have scope here?
					s_msg_id = PING_REPLY;
					Driver(CAN_TX, &e_message_array[0], &s_msg_id);
					usleep(100);
					ret = Driver(CAN_RX, s_message_array, &s_msg_id);
					if (ret != 0)
						return -1;
					if ((s_msg_id == ACK_ID)&&(s_message_array[0]==local_id)) {
						mapped = 1;
						time(&updatetv);
					}
				}
				break;
		}
	} while (s_msg_id != 0);
	return 0;
}
