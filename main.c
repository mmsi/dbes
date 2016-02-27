/***************************************************************************
 *            main.c
 *
 *  Mon Mar  9 10:00:59 2009
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

#define _GNU_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/mman.h>
#include<ctype.h>
#include<unistd.h>
#include<time.h>
#include"include/control_str.h"

#define DIO_PAGE     	0x80840000
#define JUMPER6_PAGE	0x22800000
#define JUMPER_PAGE		0x10800000
#define POS			 	0
#define PRES		 	1
#define DETENT_MIN		3
#define DETENT_MAX		10

/*prototypes*/
int MainLoop();
void Contingency(void);
void Configuration(void);

struct status_t status;
struct cnt_template_t local_control;
struct calib_t calib;
int devmem;
char *start;
int h_status;
char local_id = 0;


int Manual(void)
{
	//local_control.function = 0x0;
	//local_control.rate = 255;
	//Sensors(1, &status, &calib);
	if (detent == 1) {
		if (((status.d_input & DOWN) == 0x0) && (difftime(time(NULL),\
		      updatetv) > DETENT_MAX)) {
			local_control.function = 0x8;
			detent = 0;
		} else if ((status.d_input & UP) == 0x0) {
			local_control.function = 0x8;
			detent = 0;
		}
	} else if (diff_flag == 1) {
		if ((status.d_input & DOWN) == 0x0) {
			if ((difftime(time(NULL), updatetv)) > DETENT_MIN) {
				//printf("difference = %d", updatetv);
				detent = 1;
				diff_flag = 0;
			}
		} else {
			local_control.function = 0x8; //FIXME
			diff_flag = 0;
		}
	} else {
		if ((status.d_input & UP) == 0x0)
		    local_control.function = 0xA;
		else if ((status.d_input & DOWN) == 0x0) {
		    local_control.function = 0xC;
			diff_flag = 1;
			time(&updatetv);
		} else
			local_control.function = 0x8;
	}
	//Hyd_Control(local_control);
	return 0;
}

	
int main()
{
	int i, c, man_status, detent, diff_flag;
	char *jump6;
	char *jump;
	FILE *fp;
	time_t updatetv;

	devmem = open("/dev/mem", O_RDWR|O_SYNC);

	/*init all segments*/
	start = mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
				 devmem, DIO_PAGE);
	jump6 = mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
				devmem, JUMPER6_PAGE);
	jump = mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
	    		devmem, JUMPER_PAGE);

	//XXX check this inline
	man_status = (*jump & 0x10);
	h_status = (*jump6 & 0x1);
	
	printf("initializing sensors...\n");
	Sensors(0, &status, &calib);
	local_control.function = 0x0080;
	printf("initializing controls...\n");
	Hyd_Control(local_control);
	/*init int/arb*/
	printf("initializing interface/arbitration...\n");
	Arbitor(&status, &local_control);

	//XXX h/w manual mode
	if (man_status == 0) {
		local_control.function = 0x0;
		printf("Entering Manual Test Mode\n\nto view sensor information ");
		printf("leave a console connected\nthis is not an interactive session");
		printf("\nto exit this mode power down and remove jumper 5\n");
		printf("Happy Trials To You\n");
		sleep(3);
		local_control.rate = 255;
		while (1) {
			Sensors(1, &status, &calib);
			printf("pressure: %u height: %u", status.pressure, status.elevation);
			printf(" buttons: %X\n", status.d_input);
			if (detent == 1) {
				if (((status.d_input & DOWN) == 0x0) && (difftime(time(NULL),\
				      updatetv) > DETENT_MAX)) {
					local_control.function = 0x8;
					detent = 0;
				} else if ((status.d_input & UP) == 0x0) {
					local_control.function = 0x8;
					detent = 0;
				}
			} else if (diff_flag == 1) {
				if ((status.d_input & DOWN) == 0x0) {
					if ((difftime(time(NULL), updatetv)) > DETENT_MIN) {
						//printf("difference = %d", updatetv);
						detent = 1;
						diff_flag = 0;
					}
				} else {
					local_control.function = 0x8; //FIXME
					diff_flag = 0;
				}
			} else {
				if ((status.d_input & UP) == 0x0)
				    local_control.function = 0xA;
				else if ((status.d_input & DOWN) == 0x0) {
				    local_control.function = 0xC;
					diff_flag = 1;
					time(&updatetv);
				} else
					local_control.function = 0x8;
			}
			Hyd_Control(local_control);
		}
	}
		
	//XXX h/w auto mode
	/*configuration prompt*/
	if (h_status == 1 ) {
		/*master*/
		printf("would you like to enter configuration? y/n   ");
		do {
			c = (char)getchar();
			if (c == 'y')
				Configuration();
		} while ((c != 'y') && (c != 'n'));	
		if (LoadCalib(&calib) < 0) {
			printf("\nBad config file... entering configuration anyway\n");
			Configuration();
			if (LoadCalib(&calib) < 0) {
				printf("Still has a bad config file.\n");
				printf("Filesystem needs repairs... Exiting.");
				exit(0);
			}
		}
	} else {
		/*slave*/
		if (LoadCalib(&calib) < 0) {
			printf("\nConfiguration file loading failed!!!\n");
			exit(0); //FIXME better alerting of operator, maybe over CAN
			//shut off all valves first?
		}
	}

	/*load calibration data*/
	//FIXME send pointer everytime instead of this?
	Sensors(2, &status, &calib);
		
	
		
	/*clear init*/
	local_control.function = 0x0000;

	/*Main Loop*/
	do {
		i = MainLoop();
	} while (i == 0);

	/*clean restart*/
	//system("init 6"); FIXME temporarily disabled for debugging
}

int MainLoop()
{
	static unsigned short old_dest = 0;
	
	/*status_t status population*/
	if (Sensors(1, &status, &calib) == 1) {
		printf("Sensors Error\n");
	}

	if ((local_control.function & 0x8) == 0x8) { //MANUAL
		local_control.rate = 255; //full rate
		if ((status.d_input & UP) == 0x0)
			local_control.function = 0xA;
		else if ((status.d_input & DOWN) == 0x0)
		    local_control.function = 0xC;
		else
		    local_control.function = 0x8;
		Hyd_Control(local_control);
		if (Arbitor(&status, &local_control) < 0)
			printf("Arbitor Error\n");
	} else { //AUTO
		if (Arbitor(&status, &local_control) < 0)
			printf("Arbitor Error\n");
		//if (((local_control.function & 0x6) > 0x0) && ((local_control.dest-old_dest) > 2)) {
			if (((local_control.function & 0x2) == 0x2) &&
			    ((status.elevation - status.offset) >= local_control.dest)) {
				local_control.function = 0;
			}

			if (((local_control.function & 0x4) == 0x4) &&
			    ((status.elevation - status.offset) <= local_control.dest)) {
				local_control.function = 0;
			}
			//old_dest = local_control.dest;
		//}
			
		//printf("auto mode: calling hydraulic control\n");
		Hyd_Control(local_control);
	}
	return 0;
}

void Contingency(void)
{
	int ret;

	local_control.function = 0x0001;
	printf("entering contingency mode...\nplease correct problem");
	printf(" and reset system\n"); //FIXME ui() should handle this and the reset
	Hyd_Control(local_control); // shut off all valves
	Arbitor(&status, &local_control); //send contingency over CAN

	/*reset handler*/
	printf("press 'r' to reset: ");
	do {
		ret = fgetc(stdin);
	} while (ret != 'r');
	local_control.function = 0x0000;
	printf("\n\nresuming normal mode...\n");

	return ;
}

void Configuration(void)
{
	int raw, t, i;
	double raw_d;
	int c;
	char *line = NULL;
	size_t len = 0;
	FILE *fp;

	//FIXME disable previous noncanonical?

	fp = fopen(CONFIG_NAM, "r+");
	if (fp < 0) {
		printf("\nfopen failed\n");
	}
	setvbuf(fp, NULL, _IONBF, BUFSIZ);

	/* Address																  *
	 * We will no longer edit the address from here, it will be done manually *
	 * by editing the config file											  */

	/*jump past first line in the file*/ 
	fgets(line, 25, fp);	
	
	/*Pressure Transducer*/
	/*offset*/
	printf("\nset system pressure to zero and press enter\n");
	getline(&line, &len, stdin);
	raw = Sensor_cal(PRES);
	printf("zero set at %u\n", raw);
	fprintf(fp, "%5u\n", raw);

	/*scale*/
	printf("disconnect return hose\n");
	printf("set system to full pressure and enter the gauge ");
	printf("reading as PSI 'xxxx':  ");
	while (getline(&line, &len, stdin) != 5)
		printf("\ntry again, 'xxxx':  ");
	raw_d = ((double)Sensor_cal(PRES) / strtod(line, NULL));
	printf("\nraw = %4.3f", raw_d);
	fprintf(fp, "%4.3f\n", raw_d);

	/*Position Transducer*/
	/*offset*/
	printf("\npress enter when jack is down");
	local_control.function = 0x4;
	local_control.dest = 0x0;
	local_control.rate = 0xFF;
	Hyd_Control(local_control);
	getline(&line, &len, stdin);
	local_control.function = 0x0;
	Hyd_Control(local_control);
	raw = Sensor_cal(POS);
	printf("\nzero at %u\n", raw);
	fprintf(fp, "%5u\n", raw);

	/*scale*/
	printf("press enter when height is 24 to 36 inches\n");
	local_control.function = 0x2;
	local_control.rate = 0xFF;
	Hyd_Control(local_control);
	getchar();
	local_control.function = 0x0;
	Hyd_Control(local_control);
	printf("enter height as xx.xx:   ");
	while (getline(&line, &len, stdin) != 6) //FIXME add string tests
		printf("\ntry again, 'xx.xx'");
	raw_d = ((((double)Sensor_cal(POS)) - ((double)raw)) / strtod(line, NULL));
	fprintf(fp, "%4.3f\n", raw_d);

	local_control.function = 0x4;
	local_control.rate = 0xFF;
	Hyd_Control(local_control);
	printf("\n\ncalibration complete\n");

	fclose(fp);
}

int LoadCalib(struct calib_t *calib)
{
	int i;
	FILE *fp;
	char line[25];

	fp = fopen(CONFIG_NAM, "r");
	if (fp == NULL) {
		return -1;
		//FIXME perror?
	}
	for (i=0; i<6; i++) {
		if (fgets(line, 25, fp) == NULL)
			return -1;
		switch (i) {
			case 0:
				calib->id = strtol(line, NULL, 10);
				break;

			case 1:
				calib->pres_off = strtol(line, NULL, 10);
				break;

			case 2:
				calib->pres_scal = strtof(line, NULL);
				break;

			case 3:
				calib->height_off = strtol(line, NULL, 10);
				//printf("height offset: %i\n", calib->height_off);
				break;

			case 4:
				calib->height_scal = strtof(line, NULL);
				//printf("height scale: %f\n", calib->height_scal);
				break;

			case 5:
				status.offset = (short)strtol(line, NULL, 10);
				break;

			default:
				break;
		}
	}
	fclose(fp);	
	return 0;
}
