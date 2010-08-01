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

	do { //FIXME need to pause within this loop to allow all jacks to respond
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
						break;
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
