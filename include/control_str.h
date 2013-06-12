/**
 *dbes/control_str.h
 *origin 2-24-09
 *modified 10-6-09
 *jrm<jrcowboy79@gmail.com>
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
 
/**
 *cnt_template_t
 *unsigned char function		address and/or other information
 *		bit 0			       	contingency flag
 *		bits 1,2		   		lift - 00: idle, 01: lift, 10: lower
 *		bit 3			       	control flag - 0: auto, 1: manual
 *      bit 4                	contingency recovery
 * 		bit 5					zeroing flag
 *		bit 6		   			undefined
 *		bit 7			       	init flag
 *unsigned char	id		   		jack id
 *unsigned short dest			destination height
 *unsigned char rate			lifting rate - raw linear 0-255
 */

/*main control structure type*/
struct cnt_template_t {
	unsigned char function;
	unsigned char id;
	unsigned short dest;
	unsigned char rate;
};

/**
 *status_t
 *unsigned short elevation		elevation
 *unsigned short offset			elevation zero offset
 *unsigned short pressure		pressure transducer
 *unsigned char d_input			button and limit switches
 *		bit 0			down button
 *		bit 1			n/a
 *		bit 2			n/a
 *		bit 3			up button
 *		bit 4			tilt interlock switch
 *		bits 5-7		undefined
 */

/*individual jack status type*/
struct status_t {
	unsigned short elevation;
	unsigned short offset;
	unsigned short pressure;
	unsigned char d_input;
};

/*calibration data structure type*/
struct calib_t {
	int id;
	int pres_off;
	float pres_scal;
	int height_off;
	float height_scal;
};

#define	DOWN	0x1
#define	UP		0x8
#define INTERL	0x10

/*global file descriptor for mmap to use*/
extern int devmem;

/*hierarchy status on system*/
extern int h_status;

/*local jack address*/
extern char local_id;
