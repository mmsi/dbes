/***************************************************************************
 *            dbes/hyd_control/pwm.c
 *
 *  Tue Mar  3 10:55:58 2009
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
#include"../include/control_str.h"

#define XDIO_PAGE 0x12C00000
#define REG_0 0
#define REG_1 1
#define REG_2 2
#define REG_3 3

/**
 *rate - full scale char
 *mode - 0 = init, 1 = off, 2 = on
 */
int PWM(unsigned char rate, int mode)
{
	static unsigned int *xdio_mode, *pwm_msbs, *pwm_high_lsb, *pwm_low_lsb;
	unsigned char *start;
	unsigned short rate_s;
	
	switch (mode) {
		case 0: //init
			start = mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED,
						 devmem, XDIO_PAGE);
			/*offsets*/
			xdio_mode = (unsigned int *)(start + REG_0);
			pwm_msbs = (unsigned int *)(start + REG_1);
			pwm_high_lsb = (unsigned int *)(start + REG_2);
			pwm_low_lsb = (unsigned int *)(start + REG_3);
			
			/*set xdio control mode*/
			*xdio_mode =  0xC3;
			break;
			
		case 1: //off
			*xdio_mode = (*xdio_mode & 0xFD);
			break;
			
		case 2: //on
			/*dither*/
			//FIXME
			
			/*set rate*/
			rate_s = ((unsigned short)rate * 16);
			*pwm_high_lsb = (rate_s & 0xFF);
			*pwm_msbs = (((rate_s & 0xF00) >> 4)+(((0xFFF - rate_s)\
												   & 0xF00) >> 8));
			*pwm_low_lsb = ((0xFFF - rate_s) & 0xFF);

			/*enable*/
			if ((*xdio_mode & 0x2) == 2)
				*xdio_mode = (*xdio_mode + 0x2);
	}
	
	return 0;
}