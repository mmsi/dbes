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

#include"../include/control_str.h"
#include"include/intarb.h"
#include"include/candriver.h"
#include"../include/unique.h"

int Slave(struct cnt_template_t *local)
{
	int ret;
	char s_message_array[MSG_LENGTH];
	unsigned long s_msg_id;

	/*contingency bit check*/
	if ((local->function & 0x1) == 0x1) {
	    /*recovery*/
	    if ((local->function & 0x11) == 0x11) {
	    	do {
	    		if (Driver(CAN_RX, &s_message_array[0], &s_msg_id) != 0)
	    			return -1;
	    	} while (s_msg_id != CONT);
		return 0;
		}
		s_msg_id = CONT;
		if (Driver(CAN_TX, &s_message_array[0], &s_msg_id) != 0)
			return -1;
		return 0;
	}
		
	/*normal slave operations*/
	do {
		ret = Driver(CAN_RX, &s_message_array[0], &s_msg_id);
		if (ret != 0)
			return -1;
		switch (s_msg_id) {
			case CONTROL:
				control.function = s_message_array[0];
				control.function = ((control.function << 8) + s_message_array[1]);
				control.dest = s_message_array[2];
				control.dest = ((control.dest << 8) + s_message_array[3]);
				control.rate = s_message_array[4];
				break;
			case STATUS_REQ:
				s_message_array[0] = local_id;
				s_message_array[1] = (status_table[0].elevation >> 8);
				s_message_array[2] = (char) status_table[0].elevation;
				s_message_array[3] = (status_table[0].offset >> 8);
				s_message_array[4] = (char) status_table[0].offset;
				s_message_array[5] = (status_table[0].pressure >> 8);
				s_message_array[6] = (char) status_table[0].pressure;
				s_msg_id = STATUS;
				if (Driver(CAN_TX, &s_message_array[0], &s_msg_id) != 0)
					return -1;
				break;
			case ZERO:
				return 2;
				
			case CONT:
			    return -1;
		}
	} while (s_msg_id != 0);
	return 0;
}
