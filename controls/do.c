/***************************************************************************
 *            dbes/hyd_control/do.c
 *
 *  Fri Feb 27 17:14:16 2009
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
#include<sys/mman.h>
#include"include/control.h"
#include"../include/control_str.h"


extern char *start;

/**
 *uses ep9302 port B
 *pin variable 1,2,4,8,16,32,64,128 corresponds to DIO_0 thru DIO_7
 *as defined in control.h
 */
int Dout(int pin, int state)
{
	static unsigned int *pbdr, *pbddr;
	
	/*initialization*/
	if (pin == 0) {		
		/*offsets*/
		pbdr = (unsigned int *)(start + 0x04);
		pbddr = (unsigned int *)(start + 0x14);
		
		*pbddr = 0xFF;
		//FIXME turn off all valves on init
		return 0;
	}
	
	/*toggle pin*/
	if (state == ON) {
		*pbdr = (*pbdr | pin);
	} else if (state == OFF) {
		*pbdr = (*pbdr & (~pin));
	} else {
		printf("digital output error\n");
		return -1;
	}
	
	return 0;
}
