#include<intarb.h>

int Master(struct cnt_template_t *local, struct cnt_template_t ui)
{
	int ret=0;
	static int ui_flag;
	char m_message_array[5];
	unsigned long m_msg_id;
	struct cnt_template_t m_ui;

	/*contingency bit check*/
	if (local->function & 0xFFFE == 0xFFFF) {
		m_msg_id = 3;
		m_message_array[] = 0;
		ret = Driver(2, &m_message_array[], &m_msg_id);
		if (ret != 0) {
			return -1;
		} else {
			control.function = 1;
			return 0;
		}
	}

	/*local ui retrieval*/
	if (ui_flag == 1) {
		ControlUpdate(ui);
	do {
		ret = Driver(1, &m_message_array[], &m_msg_id);
		switch (m_msg_id) {
			case 0: //master ping
				m_msg_id = 7;
				m_message_array[] = 0;
				ret = Driver(2, &m_message_array[], &m_msg_id);
				break;

			case 3: //contingency broadcast
				m_msg_id = 3;
				m_message_array[] = 0;
				ret = Driver(2, &m_message_array[], &m_msg_id);
				if (ret != 0) {
					return -1;
				} else {
					control.function = 1;
				}
				return 0;

			case 5: //status response
				
		}
	}
	while (ret != 4);
		ret = Driver(1, &m_message_array[], &m_msg_id);
		if (ret != 0) {
			return -1;
		} else {
			
		if (m_ui.function & 0xFFF7 == 0xFFFF) {
		
