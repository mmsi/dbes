/***************************************************************************
 *            dbes/hyd_control/DO.c
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

#define DIO_PAGE 0x80840000

/**
 *uses port b
 *pin variable 0-7 corresponds to DIO_0 thru DIO_7
 */
int Dout(int pin, int state)
{
	static unsigned int *pbdr, *pbddr;
	static unsigned char *start;
	
	/*initialization*/
	if (pin == 0) {
		start = mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED,
					 devmem, DIO_PAGE);
		
		/*offsets*/
		pbdr = (unsigned int *)(start + 0x04);
		pbddr = (unsigned int *)(start + 0x14);
		
		*pbddr = 0xFF;
		//FIXME turn off all valves on init
		return 0;
	}
	
	/*toggle pin*/
	if (state == ON) {
		*pbdr = (*pbdr + pin); //FIXME if this is already on adding will be bad
	} else if (state == OFF) {
		*pbdr = (*pbdr & (~pin));
	} else {
		printf("digital output error, run and cry\n");
		return -1;
	}
	
	return 0;
}
