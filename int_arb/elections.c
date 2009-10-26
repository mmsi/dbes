/**
 *int_arb/elections.c
 *2-16-09
 *modified 9-27-09
 *jrm
 *master/slave selection is now done with jumpers
 *but I still need a place to build the jack database
 *so... laziness retains the name 'elections.c'
 */

#include<unistd.h>
#include<stdio.h>
#include"include/intarb.h"
#include"include/candriver.h"
#include"../include/unique.h"

#define PING_WAIT 2

int JackDatabase();

char e_message_array[MSG_LENGTH];
unsigned long e_msg_id;
char active_table[];
int master_flag;

/*return: 0=error, 1=slave, 2=master*/
int Elections()
{
	int i = 0;
	int ret;

	/*initialize CAN*/
	ret = Driver(INI, &e_message_array[0], &e_msg_id);

	return 1;
}

int JackDatabase()
{
	int i;
	int ret;
	
	/*broadcast id*/
	printf("broadcasting id...\n");
	e_message_array[0] = UNIQ_ID;
	e_msg_id = BROAD_ID;
	ret = Driver(TX, &e_message_array[0], &e_msg_id);
	if (ret != 0)
		printf("CAN error...\n");
	//	return 0;

	do {
		ret = Driver(RX, &e_message_array[0], &e_msg_id);
		if (ret != 0)
			printf("CAN error...\n");
		//	return 0;
		switch (e_msg_id) {
			case 7: //master already exists
				master_flag = 1;
				break;

			case 2: //build database of jacks on net
				i = 0;
				do {
					if (e_message_array[0] == jack_lookup_table[i]) {
						break; //FIXME can i break out this deeply nested?
					}
					i++;
				} while(jack_lookup_table[i] != 0);
				printf("found jack #%i\n", jack_lookup_table[i]);
				jack_lookup_table[i] = e_message_array[0];
				active++;
		}	
	} while(e_message_array[0] != 0);
	return;
}
