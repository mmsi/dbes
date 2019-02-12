/**
 *int_arb/elections.c
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
 
/**
 *master/slave selection is now done with jumpers
 *but I still need a place to build the jack database
 *so... laziness retains the name 'elections.c'
 */

#include<unistd.h>
#include<stdio.h>
#include"../include/control_str.h"
#include"include/intarb.h"
#include"include/candriver.h"

#define PING_WAIT 2

int JackDatabase();
int Broadcast(void);

unsigned char e_message_array[MSG_LENGTH];
unsigned long e_msg_id;
char active_table[];
int master_flag;


int Elections()
{
	int ret;

	/*initialize CAN*/
	ret = Driver(CAN_INI, e_message_array, &e_msg_id);

	return 0;
}


int JackDatabase()
{
	int i;
	int ret;

	jack_lookup_table[0] = local_id;
	active++;
	printf("local id = %i\n", local_id);

	do { //FIXME need to pause within this loop to allow all jacks to respond
		ret = Driver(CAN_RX, &e_message_array[0], &e_msg_id);
		if (ret != 0)
			printf("CAN error...\n");
		//	return 0;
		switch (e_msg_id) {
			case 7: //master already exists
				master_flag = 1;
				break;

			case BROAD_ID: //build database of jacks on net
				i = 0;
				do { //walk the jack table
					if (e_message_array[0] == jack_lookup_table[i]) {
						break;
					}
					i++;
				} while(jack_lookup_table[i] != 0);
				printf("found jack #%i", jack_lookup_table[i]);
				printf("   jack address: %i\n", e_message_array[0]);
				jack_lookup_table[i] = e_message_array[0];
				active++;
				e_msg_id = ACK_ID;
				Driver(CAN_TX, &e_message_array[0], &e_msg_id);
		}	
	} while(e_message_array[0] != 0);
	return 0;
}


int Broadcast(void)
{
	do {
		e_message_array[0] = local_id;
		e_msg_id = BROAD_ID;
		Driver(CAN_TX, &e_message_array[0], &e_msg_id);
		sleep(1);
		Driver(CAN_RX, &e_message_array[0], &e_msg_id);
	} while ((e_msg_id != ACK_ID) && (e_message_array[0] != local_id));
}
