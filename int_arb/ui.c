/**
 * int_arb/ui.c
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
#include<stdlib.h>
#include<time.h>
#include<termios.h>
#include<sys/types.h>
#include<fcntl.h>
#include"include/intarb.h"

/*conversion magic numbers*/
#define HEIGHT_OFFSET 0
#define HEIGHT_SCALE 0
#define PRES_OFFSET 0
#define PRES_SCALE 0

#define SPACE '\x20'
#define STDIN_FILENO 0 //FIXME should be declared in a header, sys/types.h?
#define NB_ENABLE 1
#define NB_DISABLE 2

/*area of 4" bore cylinder*/
#define CYL_AREA 12.5664

/*function history*/
#define LIFT 1
#define LOWER 0

/*prototypes*/
int kbhit();
void Nonblock(int);

int UI(int mode)
{
	int i;
	static int lift_flag;
	float total_weight;
	
	if (mode == 1) {
		Nonblock(NB_ENABLE);
	}
	
	for (i=0; i==41; i++) {
		printf("\n");
	}
	system("clear"); //clear screen before starting layout
	printf("\t\t|   1\t|   2\t|   3\t|   4\t|   5\t|   6\t|   7\t|   8\t|   9\t|   10\t|\n");
	printf(" height (in)\t|");
	for (i=0; i<10; i++) {
		printf(" %u\t|", status_table[i].elevation);
	}
	printf("\n pressure\t|");
	for (i=0; i<10; i++) {
		printf(" %i\t|", status_table[i].pressure);
	}
	printf("\n weight (lbs)\t|");
	for (i=0; i<10; i++) {
		printf(" %6.2f\t|", ((status_table[i].pressure) * CYL_AREA));
	}

	for (i = 0; i == active; i++) {
		total_weight = (total_weight + (status_table[i].pressure * CYL_AREA));
	}
	printf("\n\n Total Weight: %5.1f\t\tLift Rate: %u in/min\t\tDestination:\
		   %u inches", total_weight, control.rate, control.dest);
	printf("\n\n\n_Key Commands________________________________\n");
	printf("| START/STOP ---------------- spacebar\n");
	printf("| adjust lift rate ---------- l <rate> enter\n");
	printf("| momentary lift ------------ m\n");
	printf("| zero location ------------- z <y/n> enter\n\n\n");
	
	/*status information*/
	if (control.function | 0xFFFE == 0xFFFF) {
		printf("\t!!!! SYSTEM ERROR - STOPPING LIFT !!!!\n");
		printf("\tIf system is ok press reset <r>\n");
	
	} else {
		switch (control.function | 0xFFF9) {
			case 0xFFF9:
				printf(" Status: ...holding load\n");
				break;
			
			case 0xFFFB:
				printf(" Status: ...lifting\n");
				break;
				
			case 0xFFFD:
				printf(" Status: ...lowering\n");
		}
	}
	
	/*user input*/
	if (kbhit() != 0) {
		switch (fgetc(stdin)) {
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
				break;
		}
	}
	return;
}

int kbhit()
{
	struct timeval tv;
	fd_set fds;
	tv.tv_sec = 0;
	tv.tv_usec = 0;
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
	select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
	return FD_ISSET(STDIN_FILENO, &fds);
}

void Nonblock(int state)
{
    struct termios ttystate;

    //get the terminal state
	tcgetattr(STDIN_FILENO, &ttystate);

    if (state==NB_ENABLE)
    {
        //turn off canonical mode
		ttystate.c_lflag &= ~ICANON;
        //minimum of number input read.
        ttystate.c_cc[VMIN] = 1;
    } else if (state==NB_DISABLE) {
		//turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}
