/**
 *sensors/adc/adc_main.c
 *origin 2-25-09
 *updated 2-26-09
 *jrm<jrcowboy79@gail.com>
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
 
#include<stdio.h>
#include<sys/mman.h>
#include"peekpoke_dbes.h"
//#include"eeprog_dbes.h"
#include"ep93xx_adc_dbes.h"
#include"../include/control_str.h"

static unsigned long adc_page, syscon_page;

int ADC(unsigned short channel, int *result)
{
	static int init = 0;
	int i, adc_result[5];
	
	/*initialization*/
	if (channel == 0) {
		
		adc_page = (unsigned long)mmap(0, getpagesize(), PROT_READ|PROT_WRITE,
				MAP_SHARED, devmem, ADC_PAGE);
		if (&adc_page == MAP_FAILED) {
			printf("adc_page mmapping failed");
			return -1;
		}
			
		syscon_page = (unsigned long)mmap(0, getpagesize(), PROT_READ|PROT_WRITE,
				MAP_SHARED, devmem, SYSCON_PAGE);
		if (&syscon_page == MAP_FAILED) {
			printf("syscon_page mmaping failed");
			return -1;
		}
		
		init_ADC(adc_page, syscon_page);
		return 0;
	}
	
	/*read adc channel*/
	read_channel(adc_page, channel);
	
	read_channel(adc_page, channel);
	
	for (i = 0; i < 5; i++) {
		//usleep(); delay if needed
		adc_result[i] = read_channel(adc_page, channel);
	}
	
	/*average*/
	for (i = 0; i < 5; i++) {
		*result = (*result + adc_result[i]);
	}
	*result = (*result / 5);
	return 0;
}
