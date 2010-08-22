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

#include"../include/control_str.h"
#include"include/intarb.h"
#include"include/candriver.h"

#define MAXSTAT 20
#define UPDATE	20

char m_message_array[MSG_LENGTH];
unsigned long m_msg_id;

int Master(struct cnt_template_t *local)
{
	int ret=0, i=0, ii=0, cnt_update;
	
	struct cnt_template_t m_ui;

	/*contingency bit check*/
	if ((local->function & 0x1) == 0x1) {
	    /*recovery bit check*/
	    if ((local->function & 0x11) == 0x11) {
	        m_msg_id = CONT_RECOV;
	        if (Driver(CAN_TX, &m_message_array[0], &m_msg_id) != 0)
	            return -1;
	        return 0;
		} else {
    		m_msg_id = CONT;
	    	if (Driver(CAN_TX, &m_message_array[0], &m_msg_id) != 0) //FIXME need handlers
	    	//for different errors and return codes
	    		return -1;
    		return 0;
	    }
	}

	/*local ui retrieval*/
	ControlUpdate(&ui);

	if ((control.function & 0x20) == 0x20) {
		m_msg_id = ZERO;
		if (Driver(CAN_TX, &m_message_array[0], &m_msg_id) != 0)
			return -1;
		control.function = (control.function - 0x20);
	}

	do {
		ret = Driver(CAN_RX, &m_message_array[0], &m_msg_id);
		if (ret != 0)
			return -1;
		switch (m_msg_id) {
			case MASTER_PING: //master ping
				m_msg_id = PING_REPLY;
				m_message_array[0] = 0;
				ret = Driver(CAN_TX, &m_message_array[0], &m_msg_id);
				break;

			case CONT: //contingency broadcast
				/*
				 *is a contingency re-broadcast necessary?
				
				m_msg_id = CONT;
				m_message_array[0] = 0;
				ret = Driver(TX, &m_message_array[0], &m_msg_id);
				if (ret != 0) {
					return -1;
				} else {*/
					control.function = 1;
				return 0;
			//FIXME add status request hanlder
			case STATUS: //status response
				for (i=0; ii==1; i++) { // calculate array offset by jack address
					if (jack_lookup_table[i] == m_message_array[0])
						ii = 1;
				} //FIXME add nonexistent address handler
				status_table[i].elevation = m_message_array[1];
				status_table[i].elevation = ((status_table[i].elevation << 8) + m_message_array[2]);
				status_table[i].offset = m_message_array[3];
				status_table[i].offset = ((status_table[i].offset << 8) + m_message_array[4]);
				status_table[i].pressure = m_message_array[5];
				status_table[i].pressure = ((status_table[i].pressure << 8) + m_message_array[6]);
				break;

			case UIPASS: //remote ui retrieval (depracated)
				m_ui.function = m_message_array[0];
				m_ui.function = ((m_ui.function << 8) + m_message_array[1]);
				m_ui.dest = m_message_array[2];
				m_ui.dest = ((m_ui.dest << 8) + m_message_array[3]);
				m_ui.rate = m_message_array[4];
				ControlUpdate(&m_ui);
				
		}

	} while (m_msg_id != 0);

	if (cnt_update > UPDATE) {
		m_message_array[0] = (control.function >> 8);
		m_message_array[1] = (char) control.function;
		m_message_array[2] = (control.dest >> 8);
		m_message_array[3] = (char) control.dest;
		m_message_array[4] = control.rate;
		m_msg_id = CONTROL;

		ret = Driver(CAN_TX, &m_message_array[0], &m_msg_id);
		if (ret != 0)
			return -1;
		cnt_update = 0;
	} else {
		cnt_update++;
		StatusScheduler();
	}
}

int ControlUpdate(struct cnt_template_t *l_ui)
{
	control = *l_ui;
	return;
}

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
			id = 0;
	} else {
		i++;
	}
	return;
}
