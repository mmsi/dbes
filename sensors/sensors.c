/***************************************************************************
 *            sensors/sensors.c
 *
 *  Fri Feb 27 11:23:38 2009
 *  Copyright  2009  Joel Morgan
 *  <jrcowboy79@gmail.com>
 ****************************************************************************/

/**
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
#define POS 		0
#define PRES		1

/*Function Prototypes*/
int Sensor_cal(int);
float sensor_convdata(unsigned long, float, int);

unsigned long adc_result;

int Sensors(int mode, struct status_t *sensor_data, struct calib_t *calib)
{
	int ret;
	
	static int p_offset;
	static int h_offset;
	static float p_scale;
	static float h_scale;

	/*load calibration data*/
	if (mode == 2) {
		p_offset = calib->pres_off;
		h_offset = calib->height_off;	
		h_scale = calib->height_scal;
		p_scale = calib->pres_scal;
	}	
		
	/*initialize*/
	if (mode < 1) {
		ret = DI(0);
		if (ret != 0) {
			printf("error initializing port A...\n");
		}
		
		ret = ADC(0, &adc_result);
		if (ret != 0) {
			printf("error initializing ADC...\n");
		}
		printf("Sensors successfully initialized...\n");

		ADC(PR_TRANS, &adc_result);
		printf("raw pressure transducer value: %X\n", (unsigned int)adc_result);
		ADC(CET, &adc_result);
		printf("raw cable extension transducer value: %X\n", (unsigned int)adc_result);
		return 0;
	}
	
	/*  read buttons and switches  */
	sensor_data->d_input = DI(1);
	
	/*  read sensors  */
	/*pressure*/
	int pressure;
	if (ADC(PR_TRANS, &adc_result) != 0) {
		printf("ADC read error");
	} else {
		/*convert to psi*/ //FIXME
		pressure = sensor_convdata(adc_result, p_scale, p_offset);
		sensor_data->pressure = (unsigned short)adc_result;
	}

	/*elevation*/
	int inches;
	if (ADC(CET, &adc_result) != 0) {
		printf("ADC read error");
	} else {
		/*convert to inches*/ //FIXME add conversion algorithm
		sensor_data->elevation = (short)(sensor_convdata(adc_result, h_scale, h_offset)*100);
	}
	return 0;
}

int Sensor_cal(int mode)
{
	if (mode == POS) {
		ADC(CET, &adc_result);
		return (int)adc_result;
	} else if (mode == PRES) {
		ADC(PR_TRANS, &adc_result);
		return (int)adc_result;
	} else
		return (DI(1));
}

float sensor_convdata(unsigned long adc_data, float scale, int offset)
{
	float ret_var = 0;
	if ((scale !=0) && (offset != 0)) {
		ret_var = ((float)adc_data - (float)offset) / scale;
	}
	//printf("offset = %i\n", offset);
	//printf("scale = %f\n", scale);
	//printf("inches return = %f\n", ret_var);
	return ret_var;
}
