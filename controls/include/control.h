/***************************************************************************
 *            dbes/hyd_control/control.h
 *
 *  Fri Feb 27 17:18:55 2009
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
 
/*digital output defines*/
#define ON 1 
#define OFF 0

/*pin assignments*/
/*powers of 2 for bitwise manipulation*/
#define LIFT 1	  //DIO_0 proportional valve select. off=lower, on=lift (CR1)
#define CHECK 2	  //DIO_1 cylinder check valve (CR2)
#define MASTER 4  //DIO_2 master control valve (CR3)
#define AUX_PWM 8   //DIO_3 temporary lift/lower signal until pwm is complete (CR4)
#define SAFE 16    //DIO_4 safe signal (CR5)
