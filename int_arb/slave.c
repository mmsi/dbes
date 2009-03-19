/**
 *slave.c
 *2-15-09
 *jrm
 *
 */

#include"include/intarb.h"
#include"include/candriver.h"
#include"../include/unique.h"

int Slave(struct cnt_template_t *local)
{
	int ret;
	char s_message_array[MSG_LENGTH];
	unsigned long s_msg_id;

	/*contingency bit check*/
	if (status_table[0].function | 0xFFFE == 0xFFFF) {
		s_msg_id = 3;
		s_message_array[0] = 0;
		ret = Driver(TX, &s_message_array[0], &s_msg_id);
		if (ret != 0)
			return -1;
		do {
			ret = Driver(RX, &s_message_array[0], &s_msg_id);
			if (ret != 0)
				return -1;
		} while (s_message_array[0] != 0);
		return 0;
	} else {
		do {
			ret = Driver(RX, &s_message_array[0], &s_msg_id);
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
					s_message_array[0] = UNIQ_ID;
					s_message_array[1] = status_table[0].function;
					s_message_array[2] = (status_table[0].elevation >> 8);
					s_message_array[3] = (char) status_table[0].elevation;
					s_message_array[4] = (status_table[0].pressure >> 8);
					s_message_array[5] = (char) status_table[0].pressure;
					s_msg_id = STATUS;
					ret = Driver(TX, &s_message_array[0], &s_msg_id);
					if (ret != 0)
						return -1;
			}
		} while (s_message_array[0] != 0);

		if (ui_flag == 1) {
			s_message_array[0] = (ui.function >> 8);
			s_message_array[1] = (char) ui.function;
			s_message_array[2] = (ui.dest >> 8);
			s_message_array[3] = (char) ui.dest;
			s_message_array[4] = ui.rate;
			s_msg_id = UIPASS;
			ret = Driver(TX, &s_message_array[0], &s_msg_id);
			if (ret != 0)
				return -1;
		}
	}
	return 0;
}
