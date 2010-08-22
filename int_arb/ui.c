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
#include"../include/control_str.h"
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
/*lazy update delay seconds*/
#define UPDATE 2

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
	char c;
	int i;
	static int lift_flag;
	/*1=auto*/
	static int block_status, updated;
	float total_weight;
	static time_t updatetv;
	
	if (mode == INIT) //deprecated
		return 0;
	
	if (block_status == 0) {
		Nonblock(NB_ENABLE);
		block_status = 1;
	}

	/*FIXME add man/auto control lock here*/

	if ((difftime(time(NULL), updatetv)) > UPDATE) {
		system("clear");
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
		printf("\n\n Total Weight: %5.1f\t\tLift Rate: %u", total_weight, control.rate);
		printf(" in/min\t\tDestination: %u inches", control.dest);
		time(&updatetv);
		updated = 1;
	} else
		updated = 0;

	/*--Manual control--
	 *perform jack zeroing if needed*/
	if ((control.function & 0x8) == 0x8) {
		if (updated == 1) {
			printf("\n\n\n------MANUAL CONTROL------\n");
			printf("\nsetting zero postion:\n");
			printf("jacks must not be moving. press <z> then follow instructions\n");
			printf("to return to automatic control press <space>\n");
		}

		if (mode == LOC_STOP) {
			if (kbhit() != 0) {
				switch (fgetc(stdin)) {
					case 'z':
						Nonblock(NB_DISABLE);
						//FIXME flush stdin
						printf("set system to zero? <y/n>\n");
						do {
							c = getchar();
							if (c == 'y') {
								Nonblock(NB_ENABLE);
								ui.function = (ui.function & 0x20);
								return ;
							} else if (c == 'n') {
								Nonblock(NB_ENABLE);
								break;
							}
						} while ((c != 'y') && (c != 'n'));
					case SPACE:
						ui.function = (ui.function - 0x8);
				}
			}
		return 0;
		}
	}

	if (updated == 1) {
		printf("\n\n\n_Key Commands________________________________\n");
		printf("| START/STOP ---------------- spacebar\n");
		printf("| change direction ---------- d\n");
		printf("| adjust lift rate ---------- l <rate> enter\n");
		printf("| enter manual control ------ m\n\n");
		if (lift_flag == LOWER)
			printf(" Lowering mode active\n\n");
		else
			printf(" Lifting mode active\n\n");
		switch (control.function & 0x3) {
			case 0x0:
				printf(" Status: ...holding load\n");
				break;
			
			case 0x1:
				printf(" Status: ...lifting\n");
				break;
				
			case 0x2:
				printf(" Status: ...lowering\n");
		}
	}

	//FIXME this needs to be updated on analysis of contingency routing
	if ((control.function & 0x1) == 0x1) {
		printf("\t!!!! SYSTEM ERROR - STOPPING LIFT !!!!\n");
		printf("\tcheck system for problems\n");
		printf("\tIf system is ok press reset <r>\n");
	}
		
	
	/*user input*/
	if (kbhit() != 0) {
		switch (getchar()) {
			case SPACE:
				switch (control.function & 0x6) {
					case 0x0:
						if (lift_flag == LIFT)
							ui.function = (ui.function + 0x2);
						else
							ui.function = (ui.function + 0x4);
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

			case 'd': //change direction
				lift_flag = ((~lift_flag) & 0x1);
				break;
			
			case 'l': //adjust rate
				break;
			
			case 'm': //manual control
				ui.function = 0x8;
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
		ttystate.c_cc[VTIME] = 2;
    } else if (state==NB_DISABLE) {
		//turn on canonical mode
        ttystate.c_lflag |= ICANON;
    }
    //set the terminal attributes.
    tcsetattr(STDIN_FILENO, TCSANOW, &ttystate);
}
