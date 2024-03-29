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

#define _GNU_SOURCE
#include<stdio.h>
#include<unistd.h>
#include"../include/control_str.h"
#include"include/intarb.h"

#define INIT 1
#define NORM 0
#define CONT 2
#define MAX_ADDRESSABLE 255

/*declarations*/
int ui_flag;
int ini_flag = 0;
/* XXX added back in for breakage */
struct cnt_template_t ui;
int active;
int jack_lookup_table[MAX_ADDRESSABLE];


int Arbitor(struct status_t *local_status, struct cnt_template_t *local)
{
	/*external control information*/
	static struct cnt_template_t control;

	/*external ui structure*/
	//static struct cnt_template_t ui;

	/*external status information array*/
	static struct status_t status_table[MAX_ADDRESSABLE];

	/*jack addressing*/
	//static int active;
	//static int jack_lookup_table[MAX_ADDRESSABLE];
	
	int ret, i;
	char line[25];
	FILE *fp;

	// FIXME make the 0x80 a defined const
	if ((local->function & 0x80) == 0x80) {
		printf("Elections\n");
		Elections();
		control.dest = 0;
		return 0;
	}
	
	/*normal int/arb operations*/

	/*push local jack status to status array*/
	status_table[0] = *local_status;
	
	/** Master **/
	if (h_status == 1) {
		//printf("master: int/arb\n");
		//control.dest = (local_status->elevation - local_status->offset);
		control.rate = 255; //adopt full rate for now
		// FIXME another function hex needs to be a constant
		if ((local->function & 0x6) > 0)
			UI(LOC_ACTIVE, &ui, status_table);
		else
			UI(LOC_STOP, &ui, status_table);
		ret = Master(&control, status_table);
		
	/** Slave **/
	} else if (h_status == 0) {
		//printf("slave: int/arb\n");
		ret = Slave(&control, local_status);
		if (ret < 0) {
			printf("slave/CAN Error");
		}
	
	} else {
		printf("h_status variable out of bounds, please panic.");
		return 0; //FIXME returning normal, should this condition cause cont?
	}

	/** Master/Slave Common Code **/

	/* Local System Zeroing */
	if ((control.function & 0x20) == 0x20) {
		local_status->offset = local_status->elevation;
		/* clear zeroing flags */
		ui.function &= ~0x20;
		control.function &= ~0x20;
		printf("System Zeroed...\nzero set at: %i\n", local_status->offset);

		fp = fopen(CONFIG_NAM, "r+");
		if (fp == NULL) {
			printf("zeroing: cannot open file");
		}
		setvbuf(fp, NULL, _IONBF, BUFSIZ);

		for (i=0; i<=4; i++) {
			if (fgets(line, 25, fp) == NULL) {
				printf("zeroing: cannot read file");
				fclose(fp);
			}
		}

		fprintf(fp, "%04u\n", local_status->offset);
		fclose(fp);
	}

	/* Pass new control data to local jack */
	*local = control;
	
	return 0;
}
