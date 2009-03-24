/***************************************************************************
 *            dbes/sensors/di.c
 *
 *  Thu Feb 26 11:51:20 2009
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
 
//#include"sensor_struct.h"
#include<sys/mman.h>
#include"../include/control_str.h"

#define DIO_PAGE 0x80840000

extern char *start;

/**
 *caller needs to initialize by using the '0' arg
 *else all other values revert to reading hard-coded pin # for now
 *port F, pin 0
 */
int DI(pin)
{
	int init;
	static unsigned int *pfddr, *pfdr, *gpiofdb;
	//static unsigned char *start;
	
	if (pin == 0) {
		//start = mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED,
		//			 devmem, DIO_PAGE);
		//FIXME add mmap error handler
		/*offsets*/
		pfddr = (unsigned int *)(start + 0x34);
		pfdr = (unsigned int *)(start + 0x30);
		gpiofdb = (unsigned int *)(start + 0x64);
		
		*gpiofdb = (*gpiofdb & 0x1);
		//pfddr defaults to inputs
		return 0;
	}
	
	return (!(*pfdr & 0x1)); //FIXME can 'return' use an expression?
}
