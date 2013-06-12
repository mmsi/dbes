/**
 * master.c
 * if elected 'master', this routine runs to glue the ui(remote or local) to
 * other jacks
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
#include"../include/control_str.h"
#include"include/intarb.h"
#include"include/candriver.h"

#define MAXSTAT 20
#define UPDATE	40

unsigned char m_message_array[MSG_LENGTH];
unsigned long m_msg_id;

int Master(struct cnt_template_t *local)
{
	int ret=0, i=0, ii=0;
	static int cnt_update;
	
	struct cnt_template_t m_ui;

	

	/*local ui retrieval*/
	/*limit this to .function and .dest for now. Different master routines
	 *will modify the other structure members*/
	control.function = ui.function;
	control.dest = ui.dest;



	/*do { //flush CAN receive buffers, do nothing with it 
		ret = Driver(CAN_RX, m_message_array, &m_msg_id);
		if (ret != 0)
			return -1;
	} while (m_msg_id != 0);*/

	/* System Zeroing */
	if ((control.function & 0x20) == 0x20) {
		m_msg_id = ZERO;
		if (Driver(CAN_TX, m_message_array, &m_msg_id) < 0)
			return -1;
		ui.function &= ~0x20;
		return 0;
	}
	
	/* Broadcast control message */
	if (cnt_update > UPDATE) {
		m_msg_id = CONTROL;
		m_message_array[0] = control.function;
		m_message_array[1] = control.id;
		m_message_array[2] = (unsigned char)control.dest;
		m_message_array[3] = (unsigned char)(control.dest >> 8);
		m_message_array[4] = control.rate;
		//m_message_array[5] = control.dest;
		//m_message_array[6] = control.function;
		//m_message_array[7] = control.id;

		ret = Driver(CAN_TX, m_message_array, &m_msg_id);
		if (ret < 0)
			return -1;
		cnt_update = 0;
	} else {
		cnt_update++;
	}
}

/*int ControlUpdate(struct cnt_template_t *l_ui)
{
	control = *l_ui;
	return;
}*/

int StatusScheduler()
{
	static int i;
	static int id;
	int ret;

	if (i>=MAXSTAT) {
		m_message_array[0] = jack_lookup_table[id];
		m_msg_id = 4;
		ret = Driver(CAN_TX, &m_message_array[0], &m_msg_id);
		if (ret != 0)
			return -1;
		i = 0;
		id++;
		if (id == active)
			id = 1;
	} else {
		i++;
	}
	return 0;
}
