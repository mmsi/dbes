/***************************************************************************
 *            hyd_control/pwm.c
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
#include<unistd.h>
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
	static unsigned char *xdio_mode, *pwm_msbs, *pwm_high_lsb, *pwm_low_lsb;
	unsigned char *start;
	unsigned short rate_s;
	
	switch (mode) {
		case 0: //init
			start = mmap(0, getpagesize(), PROT_READ|PROT_WRITE, MAP_SHARED,
						 devmem, XDIO_PAGE);
			/*offsets*/
			xdio_mode = (start + REG_0);
			pwm_msbs = (start + REG_1);
			pwm_high_lsb = (start + REG_2);
			pwm_low_lsb = (start + REG_3);
			
			/*set xdio control mode*/
			*xdio_mode =  0xC1;
			break;
			
		case 1: //off
			*pwm_high_lsb = 0x00;
			*pwm_msbs = 0x0F;
			*pwm_low_lsb = 0xFF;
			*xdio_mode = (*xdio_mode & 0xFD);
			break;
			
		case 2: //on
			/*dither*/
			//FIXME
			
			/*set rate*/
			rate_s = ((unsigned short)rate * 16);
			*pwm_high_lsb = (char)(rate_s & 0xFF);
			*pwm_msbs = (char)(((rate_s & 0xF00) >> 4)+(((0xFFF - rate_s)\
												   & 0xF00) >> 8));
			*pwm_low_lsb = (char)((0xFFF - rate_s) & 0xFF);

			/*enable*/
			*xdio_mode = (*xdio_mode | 0x2);
	}
	
	return 0;
}
