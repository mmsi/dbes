/**
 *dbes/int_arb/intarb.c
 *2-17-09
 *jrm
 *Interface/Arbitor main operations
 */
 
#include<stdio.h>
#include"../include/intarb.h"

int Arbitor(struct status_t local_status, struct cnt_template_t *local)
{
	int ret;
	static int h_status;
	
	
	if (ini_flag == 0) {
		printf("Initializing network...  ");
		ret = Elections();
		if (ret = 0) {
			printf("FAILURE!!!\nactive jacks detected: %i\n", active);
			printf("check connections and restart system\n\n");
			exit(); //FIXME warning from compiler
		} else if (ret = 1) {
			printf("this jack is a slave\n");
		} else if (ret = 2) {
			printf("this jack is the master\n");
		} else
			return 0; //FIXME not fault tolerant?
		printf("active jacks on system: %i\n", (active+2));
		h_status = ret;
		
		/*probe for ui*/
		printf("hit the 'any' key if this is the operator...");
		pause(5);
		ret = getchar(); //FIXME this is blocking
		if (ret == 0) { //FIXME why? oh.
			ui_flag = 0;
		} else {
			ui_flag = 1;
		}
		ini_flag = 1;
	}
	
	/*push local jack status to array*/
	status_table[0] = local_status; //FIXME this should occur before elections
	
	/*normal int/arb operations*/
	if (ui_flag == 1) {
		
		UI();
	}
	
	/*master*/
	if (h_status == 1) {
		ret = Master(local);
		if (ret < 0) {
			printf("master routine error, calling contingency.");
			return -1;
		}
		
	/*slave*/
	} else if (h_status == 0) {
		ret = Slave(local);
		if (ret < 0) {
			printf("slave routine error, calling contingency.");
			return -1;
		}
		
	} else {
		printf("h_status variable out of bounds, please panic.");
		return 0; //returning normal, should this condition cause cont?
	}
	
	return 0;
}
