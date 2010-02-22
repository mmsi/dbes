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
 *unsigned short function		address and/or other information
 *		bit 0			contingency flag
 *		bits 1,2		lift - 00: idle, 01: lift, 10: lower
 *		bit 3			control flag - 0: auto, 1: manual
 *		bits 4-6		undefined
 *		bit 7			init flag
 *		byte 1			jack id
 *unsigned short dest			destination height
 *unsigned char rate			lifting rate - raw linear 0-255
 */

/*main control structure type*/
struct cnt_template_t {
	unsigned short function; //FIXME consider splitting address out to simplify parsing
	unsigned short dest;
	unsigned char rate;
};

/**
 *status_t
 *unsigned char function		undefined
 *unsigned short elevation		elevation
 *unsigned short pressure		pressure transducer
 *unsigned char d_input			button and limit switches
 *		bit 0			e-stop
 *		bit 1			up button
 *		bit 2			down button
 *		bit 3			cylinder extend limit switch
 *		bit 4			tilt interlock switch
 *		bits 5-7		undefined
 */

/*individual jack status type*/
struct status_t {
	unsigned char function;
	unsigned short elevation;
	unsigned short pressure;
	unsigned char d_input;
};

/*global file descriptor for mmap to use*/
extern int devmem;
