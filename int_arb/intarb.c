/**
 *  dbes/int_arb/intarb.c
 *  2-17-09
 *  modified 3-23-09
 *  jrm<jrcowboy79@gmail.com>
 *  Interface/Arbitor main operations
 */

/*
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

#include<stdio.h>
#include<unistd.h>
//#include"include/intarb.h"
#include"../include/control_str.h"

#define INIT 1
#define NORM 0
#define MAX_ADDRESSABLE 255

/*declarations*/
struct cnt_template_t control;
struct cnt_template_t ui;
struct status_t status_table[MAX_ADDRESSABLE];
int active;
int jack_lookup_table[MAX_ADDRESSABLE];
int ui_flag;
int ini_flag = 0;

int Arbitor(struct status_t local_status, struct cnt_template_t *local)
{
	int ret;
	static int h_status; //FIXME extern to Main
	
	
	if (ini_flag == 0) {
		printf("Initializing network...  ");
		ret = Elections();
		if (ret = 0) {
			printf("FAILURE!!!\nactive jacks detected: %i\n", active);
			printf("check connections and restart system\n\n");
			return -1;
		}
		printf("active jacks on system: %i\n", active);

		if (h_status == 1) {
			UI(INIT);
		}
		ini_flag = 1;
		return;
	}
	
	/*push local jack status to array*/
	status_table[0] = local_status;
	
	/*normal int/arb operations*/
	
	/*master*/
	if (h_status == 1) {
		UI(NORM); //FIXME only update every x loops
		ret = Master(local);
		if (ret < 0) {
			printf("master routine error, calling contingency.");
			return -1;
		}
		
	/*slave*/
	} else if (h_status == 0) {
		ret = Slave(local);
		if (ret < 0) {
			printf("slave routine error, calling contingency.");
			return -1;
		}
		
	} else {
		printf("h_status variable out of bounds, please panic.");
		return 0; //FIXME returning normal, should this condition cause cont?
	}
	
	return 0;
}
