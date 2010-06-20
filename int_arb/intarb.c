/**
 *  dbes/int_arb/intarb.c
 *  2-17-09
 *  modified 3-23-09
 *  jrm<jrcowboy79@gmail.com>
 *  Interface/Arbitor main operations
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

#include<stdio.h>
#include<unistd.h>
#include"../include/control_str.h"
#include"include/intarb.h"

#define INIT 1
#define NORM 0
#define CONT 2
#define MAX_ADDRESSABLE 255

/*declarations*/
struct cnt_template_t control;
struct cnt_template_t ui;
struct status_t status_table[MAX_ADDRESSABLE];
int active;
int jack_lookup_table[MAX_ADDRESSABLE];
int ui_flag;
int ini_flag = 0;

int Arbitor(struct status_t *local_status, struct cnt_template_t *local)
{
	int ret;
	
	/*contingency router*/
	if ((local->function & 0x01) == 0x1) {
	    if (h_status == 1) {
	        Master(local);    
	        UI(CONT);  
	        local->function = (local->function + 0x10);\
	        Master(local); //FIXME assume global recovery or request statuses?
	        local->function = 0;
	    } else {
	        Slave(local);
	        local->function = 0;
	    }
	    return 0;
	}
	
	/*initialize*/
	if (local->function == 0x80) {
		printf("Initializing network...  ");
		ret = Elections();
		if (ret == 0) {
			printf("FAILURE!!!\nactive jacks detected: %i\n", active);
			printf("check connections and restart system\n\n");
			return -1; //FIXME this needs to return something coherent
		}
		printf("active jacks on system: %i\n", active);

		if (h_status == 1) {
			//UI(INIT);FIXME we dont want nonblock yet
		
		}
		ini_flag = 1;
		return;
	}
	
	/*push local jack status to array*/
	status_table[0] = *local_status;
	//FIXME needs .function to be locally populated
	
	/*normal int/arb operations*/
	
	/*master*/
	if (h_status == 1) {
		if ((local->function & 0x6) > 0)
			UI(LOC_ACTIVE);
		else
			UI(LOC_STOP);
		ret = Master(local);
		if (ret < 0) {
			printf("master routine error, calling contingency.");
			return -1;
		}
		
	/*slave*/
	} else if (h_status == 0) {
		ret = Slave(local);
		if (ret < 0) {
			//printf("slave routine error, calling contingency.");
			return -1;
		} else if (ret == 2)
			local_status->offset = local_status->elevation;
	} else {
		printf("h_status variable out of bounds, please panic.");
		return 0; //FIXME returning normal, should this condition cause cont?
	}
	
	return 0;
}
