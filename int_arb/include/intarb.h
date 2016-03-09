/**
 *int_arb/intarb.h
 *header file for Interface/Arbitor segment
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


#define MAX_ADDRESSABLE 255

/*ui modes*/
#define NORM 	        0
#define INIT 	        1 //deprecated
#define CONTIN 	        2
#define LOC_ACTIVE		3
#define LOC_STOP		4
/*ui return*/
#define UI_ZERO			2



/*local ui 1=local, 0=remote*/
extern int ui_flag; //depracated

/*Int/Arb initialization*/
extern int ini_flag; //FIXME why is this extern?
