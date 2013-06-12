/***************************************************************************
 *            controls/hyd_control.c
 *
 *  Mon Mar  9 10:43:41 2009
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
 
#include"include/control.h"
#include"../include/control_str.h"

int Hyd_Control(struct cnt_template_t control)
{
	/*init handler*/
	if ((control.function & 0x0080) == 0x80) {
		Dout(0, 0);
		PWM(0, 0);
		return 0;
	}
	
	/*contingency handler
	if ((control.function & 0x0001) == 0x1) {
		Dout(MASTER, OFF);
		Dout(CHECK, OFF);
		PWM(0, 1);
		Dout(AUX_PWM, OFF);
		return 0;
	}*/
	
	switch (control.function & 0x0006) {
		case 0x0:
			Dout(MASTER, OFF);
			Dout(CHECK, OFF);
			Dout(AUX_PWM, OFF);
			PWM(0, 1);
			break;
			
		case 0x2: //lift
			Dout(MASTER, ON);
			Dout(CHECK, OFF);
			Dout(LIFT, ON);
			Dout(AUX_PWM, ON);
			PWM(control.rate, 2); //FIXME implement dither here
			break;
			
		case 0x4: //lower
			Dout(MASTER, ON);
			Dout(CHECK, ON);
			Dout(LIFT, OFF);
			Dout(AUX_PWM, ON);
			PWM(control.rate, 2);
	}
	return 0;
}
