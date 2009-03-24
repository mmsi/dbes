/**
 *int_arb/elections.c
 *2-16-09
 *jrm
 *elect a master in concert with other jacks on the net
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

	printf("starting elections\n");
	/*check for existing master*/
	printf("checking for master...\n");
	active = 1;
	e_msg_id = MASTER_PING;
	ret = Driver(TX, &e_message_array[0], &e_msg_id);
	if (ret != 0)
		return 0;
	
	sleep(PING_WAIT);
	JackDatabase();
	sleep(PING_WAIT);
	JackDatabase();
	
	/*elect master*/
	if (master_flag == 0) {
		do {
			if (UNIQ_ID < jack_lookup_table[i]) {
				return 1; //slave FIXME slave with local ui still needs to know th # of jacks (active)
			}
			i++;
		} while(jack_lookup_table[i] != 0);
	}
	return 2; //master
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
