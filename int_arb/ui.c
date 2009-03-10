/**
 * int_arb/ui.c
 * 2-19-09
 * jrm
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
#include<stdlib.h>
#include"intarb.h"

/*conversion magic numbers*/
#define HEIGHT_OFFSET 0
#define HEIGHT_SCALE 0
#define PRES_OFFSET 0
#define PRES_SCALE 0

#define SPACE '\x20'

/*function history*/
#define LIFT 1
#define LOWER 0

char some_array[] = {1,2,3,4,5,7,9,3,4};

int UI()
{
	int i;
	static int lift_flag;
	
	//FIXME clear screen before starting layout
	printf("\t\t|   1\t|   2\t|   3\t|   4\t|   5\t|   6\t|   7\t|   8\t|   9\t|   10\t|\n");
	printf(" height (in)\t|");
	for (i=0; i<10; i++) {
		printf(" %u\t|", i); //FIXME add arg
	}
	printf("\n pressure\t|");
	for (i=0; i<10; i++) {
		printf(" %i\t|", some_array[i]); //FIXME add arg
	}
	printf("\n weight (lbs)\t|");
	for (i=0; i<10; i++) {
		printf(" %i\t|", some_array[i]); //FIXME add arg
	}

	printf("\n\n Total Weight: %u\t\tLift Rate: %u in/min\t\tDestination: %u inches",,,); //FIXME add args
	printf("\n\n\n_Key Commands________________________________\n");
	printf("| START/STOP ---------------- spacebar");
	printf("| adjust lift rate ---------- l <rate> enter");
	printf("| momentary lift ------------ m");
	printf("| zero location ------------- z <y/n> enter\n\n\n");
	
	/*status information*/
	if (control.function | 0xFFFE == 0xFFFF) {
		printf("\t!!!! SYSTEM ERROR - STOPPING LIFT !!!!\n");
		printf("\tIf system is ok press reset <r>\n");
	
	} else {
		switch (control.function | 0xFFF9) {
			case 0xFFF9:
				printf(" Status: ...holding load");
			
			case 0xFFFB:
				printf(" Status: ...lifting");
				
			case 0xFFFD:
				printf(" Status: ...lowering");
		}
	}
	
	/*user input*/
	switch (getchar()) {
		case SPACE:
			switch (control.function & 0x6) {
				case 0x0:
					if (lift_flag == LIFT)
						ui.function = (ui.function + 0x2);
					else
						ui.function = (ui.function + 0x6);
					break;
					
				case 0x2:
					ui.function = (ui.function - 0x2);
					lift_flag = LIFT;
					break;
					
				case 0x4:
					ui.function = (ui.function - 0x4);
					lift_flag = LOWER;
			}
			break;
			
		case 'l': //adjust rate
			break;
			
		case 'm': //momentary lift FIXME structures non-supportive
			break;
			
		case 'z': //zero location
			
	}
	return;
}
