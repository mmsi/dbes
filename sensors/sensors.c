/***************************************************************************
 *            dbes/sensors/sensors.c
 *
 *  Fri Feb 27 11:23:38 2009
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
 
//#include"sens_struct.h"
#include<stdio.h>
#include"../include/control_str.h"

#define PR_TRANS	0x0640
#define CET			0x0610

int Sensors(int mode, struct status_t *sensor_data)
{
	int adc_result = 0, ret;
	
	/*initialize*/
	if (mode < 1) {
		ret = DI(0);
		if (ret != 0) {
			printf("error initializing port F...\n");
		}
		
		ret = ADC(0, &adc_result);
		if (ret != 0) {
			printf("error initializing ADC...\n");
		}
		printf("Sensors successfully initialized...\n");

		ADC(PR_TRANS, &adc_result);
		printf("raw pressure transducer value: %X\n", adc_result);
		ADC(CET, &adc_result);
		printf("raw cable extension transducer value: %X\n", adc_result);
		return 0;
	}
	
	/*E-STOP check*/
	if (DI(1) == 1) {
		return 1;
	}
	
	/*  read sensors  */
	/*pressure*/
	if (ADC(PR_TRANS, &adc_result) != 0) {
		printf("ADC read error");
	} else {
		/*convert to PSI*/ //FIXME add conversion alorithm
		sensor_data->pressure = adc_result; //FIXME will this correctly cast a int into a short?
	}
	
	/*elevation*/
	if (ADC(CET, &adc_result) != 0) {
		printf("ADC read error");
	} else {
		/*convert to inches*/ //FIXME add conversion alorithm
		sensor_data->elevation = adc_result;
	}
	return 0;
}
