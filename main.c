/***************************************************************************
 *            dbes/main.c
 *
 *  Mon Mar  9 10:00:59 2009
 *  Copyright  2009  Joel Morgan
 *  <jrcowboy79@gmail.com>
 ****************************************************************************/

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
#include<fcntl.h>
#include<sys/mman.h>
#include"include/control_str.h"

#define DIO_PAGE 0x80840000

/*prototypes*/
int MainLoop();
int Contingency(void);

struct status_t status;
struct cnt_template_t local_control; //FIXME need better scope?
int devmem;
char *start;

int main()
{
	int i;
	
	devmem = open("dev/mem", O_RDWR|O_SYNC);
	
	/*init all segments*/
	start = mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
				 devmem, DIO_PAGE);
	printf("initializing sensors...\n");
	Sensors(0);
	local_control.function = 0x0080;
	printf("initializing controls...\n");
	Hyd_Control(local_control);
	printf("initializing interface/arbitration...\n");
	Arbitor(status, &local_control);
	/*clear init*/
	local_control.function = 0x0000;
	
	/*infinite loop unless fatal error*/
	do {
		i = MainLoop();
	} while (i == 0);
	
	/*clean restart*/
	//system("init 6"); FIXME temporarily disabled for debugging
}

int MainLoop()
{
	/*status_t status population*/
	if (Sensors(1, &status) == 1) {
		Contingency();
		return 0;
	}
		
	/*get control orders*/
	if (Arbitor(status, &local_control) < 0) {
		printf("int/arb error...\n");
		Contingency();
		return 0;
	}
		
	/*actuate hydraulics*/
	Hyd_Control(local_control);

	return 0;
}

int Contingency(void)
{
	int ret;
	
	local_control.function = 0x0001;
	printf("entering contingency mode...\nplease correct problem");
	printf(" and reset system\n"); //FIXME ui() should handle this and the reset
	Hyd_Control(local_control);
	Arbitor(status, &local_control);
	
	/*reset handler*/
	printf("press 'r' to reset: ");
	do {
		ret = fgetc(stdin);
	} while (ret != 'r');
	local_control.function = 0x0000;
	printf("\n\nresuming normal mode...\n");
	 
	return;
}