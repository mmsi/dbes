/**
 * master.c
 * 2-15-09
 * jrm
 * if elected 'master', this routine runs to glue the ui(remote or local) to
 * other jacks
 */ 
#include"intarb.h"
#include"candriver.h"

#define MAXSTAT 20

char m_message_array[MSG_LENGTH];
unsigned long m_msg_id;

int Master(struct cnt_template_t *local, struct cnt_template_t ui)
{
	int ret=0, i=0, ii=0, cnt_update;
	
	struct cnt_template_t m_ui;

	/*contingency bit check*/
	if (local->function | 0xFFFE == 0xFFFF) {
		m_msg_id = 3;
		m_message_array[0] = 0; //on a cont broadcast data can be garbage
		ret = Driver(TX, &m_message_array[0], &m_msg_id);
		if (ret != 0) {
			return -1;
		} else {
			control.function = 1;
			return 0;
		}
	}

	/*local ui retrieval*/
	if (ui_flag == 1) {
		ControlUpdate(&ui);
	}

	do {
		ret = Driver(RX, &m_message_array[0], &m_msg_id);
		if (ret != 0)
			return -1;
		switch (m_msg_id) {
			case 0: //master ping
				m_msg_id = 7;
				m_message_array[0] = 0;
				ret = Driver(2, &m_message_array[0], &m_msg_id);
				break;

			case 3: //contingency broadcast
				/*
				 *is a contingency re-broadcast necessary?
				 */
				m_msg_id = 3;
				m_message_array[0] = 0;
				ret = Driver(TX, &m_message_array[0], &m_msg_id);
				if (ret != 0) {
					return -1;
				} else {
					control.function = 1;
				}
				return 0;

			case 5: //status response
				for (i=0; ii==1; i++) { // calculate array offset by jack address
					if (jack_lookup_table[i] == m_message_array[0])
						ii = 1;
				} //FIXME add nonexistent address handler
				status_table[i].function = m_message_array[1];
				status_table[i].elevation = m_message_array[2];
				status_table[i].elevation = ((status_table[i].elevation << 8) + m_message_array[3]);
				status_table[i].pressure = m_message_array[4];
				status_table[i].pressure = ((status_table[i].pressure << 8) + m_message_array[5]);
				break;

			case 6: //remote ui retrieval
				m_ui.function = m_message_array[0];
				m_ui.function = ((m_ui.function << 8) + m_message_array[1]);
				m_ui.dest = m_message_array[2];
				m_ui.dest = ((m_ui.dest << 8) + m_message_array[3]);
				m_ui.rate = m_message_array[4];
				ControlUpdate(&m_ui);
				
		}

	} while (m_message_array[0] != 0);

	if (cnt_update == 1) { //parse from control to be broadcast
		m_message_array[0] = (control.function >> 8);
		m_message_array[1] = (char) control.function;
		m_message_array[2] = (control.dest >> 8);
		m_message_array[3] = (char) control.dest;
		m_message_array[4] = control.rate;
		m_msg_id = 1;

		ret = Driver(TX, &m_message_array[0], &m_msg_id);
		if (ret != 0)
			return -1;
	} else {
		StatusScheduler();
	}
}

int ControlUpdate(struct cnt_template_t *ui)
{
	control = *ui;
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
		ret = Driver(TX, &m_message_array[0], &m_msg_id);
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
