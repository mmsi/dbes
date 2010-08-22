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
#include"include/control_str.h"

#define DIO_PAGE     	0x80840000
#define JUMPER6_PAGE	0x22800000
#define JUMPER_PAGE		0x10800000
#define POS			 	0
#define PRES		 	1

/*configuration file name*/
#define CONFIG_NAM "config"

/*length of configuration file (characters)*/
#define MAX_CONFIG 29

/*prototypes*/
int MainLoop();
void Contingency(void);
void Configuration(void);

struct status_t status;
struct cnt_template_t local_control;
int devmem;
char *start;
char calib[27];
int h_status;
char local_id = 0;

int main()
{
	int i, c, man_status;
	char *jump6;
	char *jump;
	FILE *fp;

	devmem = open("/dev/mem", O_RDWR|O_SYNC);

	/*init all segments*/
	start = mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
				 devmem, DIO_PAGE);
	jump6 = mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
				devmem, JUMPER6_PAGE);
	jump = mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
	    		devmem, JUMPER_PAGE);

	man_status = (*jump & 0x10);
	h_status = (*jump6 & 0x1);
	printf("initializing sensors...\n");
	Sensors(0);
	local_control.function = 0x0080;
	printf("initializing controls...\n");
	Hyd_Control(local_control);

	if (man_status == 0) {
		local_control.function = 0x0;
		printf("Entering Manual Test Mode\n\nto view sensor information ");
		printf("leave a console connected\nthis is not an interactive session");
		printf("\nto exit this mode power down and remove jumper 5\n");
		printf("Happy Trials To You\n");
		sleep(3);
		local_control.rate = 255;
		while (1) {
			Sensors(1, &status);
			printf("pressure: %u height: %u", status.pressure, status.elevation);
			printf(" buttons: %X\n", status.d_input);
			if ((status.d_input & 0x8) == 0x0)
			    local_control.function = 0xA;
			else if ((status.d_input & 0x1) == 0x0)
			    local_control.function = 0xC;
			else
				local_control.function = 0x8;
			Hyd_Control(local_control);
		}
	}
		
	/*configuration prompt*/
	if (h_status == 1 ) {
		/*master*/
		fp = fopen(CONFIG_NAM, "r");
		if (fp != NULL) {
			for (i=0; i<=MAX_CONFIG; i++)
				calib[i] = (char)fgetc(fp);//FIXME check for EOF or error
			fclose(fp);
		} else {
			Configuration();
			goto bypass; /*FIXME eliminate goto
			need to jump past the prompt below*/
		} //just move the block below into the if{} above
		printf("would you like to enter configuration? y/n   ");
		do {
			c = (char)getchar();
			if (c == 'y')
				Configuration();
		} while ((c != 'y') && (c != 'n'));
	} else {
		/*slave*/
		fp = fopen(CONFIG_NAM, "r");
		if (fp == NULL) {
			printf("no configuration file");
			exit(0); //FIXME better alerting of operator, maybe over CAN
		}
		for (i = 0; i <= MAX_CONFIG; i++)
			calib[i] = getc(fp);
		fclose(fp);
	}

	bypass:
	local_id = (calib[0]*100) + (calib[1]*10) + calib[2];
	printf("initializing interface/arbitration...\n");
	Arbitor(&status, &local_control);
	/*clear init*/
	local_control.function = 0x0000;

	/*infinite loop unless fatal error*/
	do {
		i = MainLoop();
	} while (i == 0);

	/*clean restart*/
	//system("init 6"); FIXME temporarily disabled for debugging
}

int MainLoop()
{
	/*status_t status population*/
	if (Sensors(1, &status) == 1) {
		Contingency();
		return 0;
	}

	if ((local_control.function & 0x8) == 0x8) {
		local_control.rate = 255;
		if ((status.d_input & 0x1) == 0x1)
			local_control.function = 0xC;
		else if ((status.d_input & 0x8) == 0x8)
		    local_control.function = 0xA;
		else
		    local_control.function = 0x8;
		Hyd_Control(local_control);
		if (Arbitor(&status, &local_control) < 0)
			Contingency();
	} else {
		if (Arbitor(&status, &local_control) < 0)
			Contingency();
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

	fp = fopen(CONFIG_NAM, "w"); //FIXME add error handler
	if (fp < 0) {
		printf("\nfopen failed\n");
	}
	setvbuf(fp, NULL, _IONBF, BUFSIZ);

	/*address*/
	printf("current address is: %.3s\n", &calib[0]);
	printf("would you like to change the address? y/n   ");
	do {
		c = getchar();
		if (c == 'y') {
			printf("\nthe address must be three numeric digits and must not ex");
			printf("ceed 255\nplease enter a three digit number:   ");
			do {
				t = 0;
				while (getline(&line, &len, stdin) != 4)
					printf("\ntry again - 3 digits, press enter!  ");
				for (i=(int)line;i<((int)line+3);i++) {
					if((isdigit((int)(*(line + t)))) == 0) {
						t++;
						printf("\nonly use digits  ");
					}
				}
			} while (t != 0);
			printf("\naddress is: %.3s", line);
			fprintf(fp, "%.3s", line);
		} else if (c == 'n')
			fprintf(fp, "%.3s", &calib[0]);
	} while ((c != 'y') && (c != 'n'));
	
	/*Pressure Transducer*/
	/*offset*/
	//FIXME flush stdin?
	printf("\nset system pressure to zero and press enter\n");
	getline(&line, &len, stdin);
	raw = Sensor_cal(PRES);
	printf("zero set at %u\n", raw);
	fprintf(fp, "%5u", raw);

	/*scale*/
	printf("disconnect return hose\n");
	printf("set system to full pressure and enter the gauge ");
	printf("reading as PSI 'xxxx':  ");
	while (getline(&line, &len, stdin) != 5)
		printf("\ntry again, 'xxxx':  ");
	raw_d = ((double)Sensor_cal(PRES) / strtod(line, NULL));
	printf("\nraw = %4.3f", raw_d);
	fprintf(fp, "%4.3f", raw_d);

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
	fprintf(fp, "%5u", raw);

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
	fprintf(fp, "%4.3f", raw_d);

	local_control.function = 0x4;
	local_control.rate = 0xFF;
	Hyd_Control(local_control);
	printf("\n\ncalibration complete\n");

	fclose(fp);
}
