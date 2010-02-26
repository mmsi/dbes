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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<sys/mman.h>
#include"include/control_str.h"

#define DIO_PAGE     0x80840000
#define JUMPER_PAGE  0x22800000
#define POS			 0
#define PRES		 1

/*configuration file name*/
#define CONFIG_NAM "config"

/*length of configuration file (characters)*/
#define MAX_CONFIG 27

/*prototypes*/
int MainLoop();
int Contingency(void);
int Configuration(void);

struct status_t status;
struct cnt_template_t local_control; //FIXME need better scope?
int devmem;
char *start;
FILE *fp;
char calib[27];

int main()
{
	int i, c;
	char *jump6;


	devmem = open("dev/mem", O_RDWR|O_SYNC);

	/*init all segments*/
	start = mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
				 devmem, DIO_PAGE);
	jump6 = mmap(0, 4096, PROT_READ|PROT_WRITE, MAP_SHARED,
				devmem, JUMPER_PAGE);

	h_status = (*jump6 & 0x1);
	printf("initializing sensors...\n");
	Sensors(0);
	local_control.function = 0x0080;
	printf("initializing controls...\n");
	Hyd_Control(local_control);
	printf("initializing interface/arbitration...\n");
	Arbitor(status, &local_control);

	/*configuration prompt*/
	if (h_status == 1 ) {
		/*master*/
		fp = fopen(CONFIG_NAM, "r");
		if (fp >= 0) {
			for (i=0; i<=MAX_CONFIG; i++)
				calib[i] = getc(fp);
			fclose(fp);
		} else {
			Configuration();
			goto bypass; /*FIXME eliminate goto
			need to jump past the prompt below*/
		}
		printf("would you like to enter configuration? y/n   ");
		while ((c = getchar()) != 'y' || 'n')
		if (c == 'y')
			Configuration();
	} else {
		/*slave*/
		fp = fopen(CONFIG_NAM, "r");
		if (fp < 0) {
			printf("no configuration file");
			exit(0); //FIXME better alerting of operator, maybe over CAN
		}
		for (i = 0; i <= MAX_CONFIG; i++)
			calib[i] = getc(fp);
		fclose(fp);
	}

	bypass:
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

	/*get control orders*/
	if (Arbitor(status, &local_control) < 0) {
		printf("int/arb error...\n");
		Contingency();
		return 0;
	}

	/*actuate hydraulics*/
	Hyd_Control(local_control);

	return 0;
}

int Contingency(void)
{
	int ret;

	local_control.function = 0x0001;
	printf("entering contingency mode...\nplease correct problem");
	printf(" and reset system\n"); //FIXME ui() should handle this and the reset
	Hyd_Control(local_control); // shut off all valves
	Arbitor(status, &local_control); //send contingency over CAN

	/*reset handler*/
	printf("press 'r' to reset: ");
	do {
		ret = fgetc(stdin);
	} while (ret != 'r');
	local_control.function = 0x0000;
	printf("\n\nresuming normal mode...\n");

	return;
}

int Configuration(void)
{
	char str[20];
	int raw;
	char c;

	fp = fopen("config", "w"); //FIXME add error handler
	if (fp < 0) {
	}

	/*address*/
	printf("current address is: %c3\n", calib[0]);
	printf("would you like to change the address? y/n   ");
	while ((c = getchar()) != 'y' || 'n') {
		printf("\njust enter 'y' or 'n', moron");
	}
	if (c == 'y') {
		printf("\nplease enter a three digit number:   ");
		while ((strlen(gets(&str))) != 3) {//FIXME check for numericness
			printf("\ntry again - 3 digits, press enter!");
		}
		fprintf(fp, "%s3", str);
	} else
		fprintf(fp, "%s3", calib[0]);
	
	/*pressure*/
	printf("\nset system pressure to zero and wait for the prompt\n");
	sleep(5);
	Sensor_cal(raw, PRES);
	printf("zero set at %u\n", raw);
	fprintf(fp, "%      ", raw);
	printf("disconnect return hose\n");
	printf("set system to full pressure and enter the gauge ");
	printf("reading\n");
	fprintf(fp, "%    ", (float)(Sensor_cal(raw, PRES) / (atoi(gets(&str)))));

	/*Position Transducer*/
	/*offset*/
	printf("\nset jack height down and press  enter");
	local_control.function = 0x4;
	local_control.dest = 0x0;
	local_control.rate = 0xFF;
	Hyd_Control(local_control);
	getchar();
	local_control.function = 0x0;
	Hyd_Control(local_control);
	raw = Sensor_cal(POS);
	printf("zero at %u\n", raw);
	//FIXME lseek() to proper place
	fprintf(fp, "%4u", raw);

	/*scale*/
	printf("press enter when height is 24 to 36 inches\n");
	local_control.function = 0x2;
	local_control.dest = 0xFFFF; //FIXME .dest useless here?
	local_control.rate = 0xFF;
	Hyd_Control(local_control);
	getchar();
	local_control.function = 0x0;
	Hyd_Control(local_control);
	printf("enter height as xx.xx:   ");
	gets(&str); //FIXME add string test
	raw = (float)(((Sensor_cal(POS)) - raw) / (atof(gets(&str))));
	fprintf(fp, "%4.3f", raw);

	local_control.function = 0x4;
	local_control.rate = 0xFF;
	Hyd_Control(local_control);
	printf("calibration complete");
}
