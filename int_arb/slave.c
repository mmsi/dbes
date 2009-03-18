/**
 *slave.c
 *2-15-09
 *jrm
 *
 */

#include"../include/intarb.h"
#include"../include/candriver.h"

int Slave(struct cnt_template_t *local)
{
	int ret
	char s_message_array[MSG_LENGTH];
	unsigned long s_msg_id;

	/*contingency bit check*/
	if (local->function | 0xFFFE == 0xFFFF) { //FIXME local status is in array now
		s_msg_id = 3;
		s_message_array[] = 0;
		ret = Driver(TX, &s_message_array[], &s_msg_id);
		if (ret != 0)
			return -1;
		do {
			ret = Driver(RX, &s_message_array[], &s_msg_id);
			if (ret != 0)
				return -1;
		} while (s_message_array[] != 0);
		return 0;
	} else {
		do {
			ret = Driver(RX, &s_message_array[], &s_msg_id);
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

				case STATUS_REQ: //FIXME rewrite, local status is now in array [0]
					s_message_array[0] = UNIQ_ID;
					s_message_array[1] = local->function;
					s_message_array[2] = (local->elevation >> 8);
					s_message_array[3] = (char) local->elevation;
					s_message_array[4] = (local->pressure >> 8);
					s_message_array[5] = (char) local->pressure;
					s_msg_id = STATUS;
					ret = Driver(TX, &s_message_array[], &s_msg_id);
					if (ret != 0)
						return -1;
			}
		} while (s_message_array[] != 0);

		if (ui_flag == 1) {
			s_message_array[0] = (ui.function >> 8);
			s_message_array[1] = (char) ui.function;
			s_message_array[2] = (ui.dest >> 8);
			s_message_array[3] = (char) ui.dest;
			s_message_array[4] = ui.rate;
			s_msg_id = UIPASS;
			ret = Driver(TX, &s_message_array[], &s_msg_id);
			if (ret != 0)
				return -1;
	}
return 0;
}
