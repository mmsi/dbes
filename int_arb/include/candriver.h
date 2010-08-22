/**
 *int_arb/candriver.h
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
 *definitions for functions interacting with Driver()
 *Byte 0: jack address
 *Byte 1: function - bit 0 contingency bit, bits 1,2 lift status
 *Bytes 2,3: 16 bit elevation
 *Byte 4: rate
 *exception - status frames(id 5) bytes 4,5: 16bits pressure 
 */

/*driver control defines*/
#define CAN_INI		0
#define CAN_RX		1
#define CAN_TX		2
#define CAN_CLOSE	3
#define CAN_FILTER	4

/*max number of CAN data bytes*/
#define MSG_LENGTH	7

/*message id*/
#define MASTER_PING			10  //no data
#define CONTROL				1	   //5 bytes
#define BROAD_ID			2	   //byte 0
#define CONT				3		   //no data
#define STATUS_REQ			4   //byte 0
#define STATUS				5	   //6 bytes - note exception
#define UIPASS				6	   //5 bytes
#define PING_REPLY			7   //no data
#define CONT_RECOV			8   //no data
#define ZERO				9
#define ACK_ID				11	//1 byte - address

//FIXME add defines for return conditions

//int Driver(unsigned char, char *, unsigned long *);
