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
 

#include<sys/mman.h>
#include"../include/control_str.h"


extern char *start;

/**
 *caller needs to initialize by using the '0' arg
 *uses ep9302 port A (ts7260 lcd header)
 *
 */

int DI(mode)
{
	int init;
	static unsigned int *paddr, *padr, *gpioadb;
	
	if (mode == 0) {
		//start = mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED,
		//			 devmem, DIO_PAGE);

		/*offsets*/
		paddr = (unsigned int *)(start + 0x10);
		padr = (unsigned int *)start;
		gpioadb = (unsigned int *)(start + 0xA8);
		
		*gpioadb = 0xF;
		//paddr defaults to inputs
		return 0;
	}
	
	return (*padr);
}
