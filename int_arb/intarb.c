/**
 *intarb.c
 *2-17-09
 *jrm
 *Interface/Arbitor main operations
 */
 
#include<intarb.h>

int Arbitor()
{
	int ret;
	static int h_status;
	
	printf("Initializing network...  ")
	if (ini_flag == 0) {
		ret = Elections();
		if (ret = 0) {
			printf("FAILURE!!!\nactive jacks detected: %i\n", active);
			printf("check connections and restart system\n\n");
			exit();
		} else if (ret = 1) {
			printf("this jack is a slave\n");
		} else if (ret = 2) {
			printf("this jack is the master\n");
		} else
			return 0;
		printf("active jacks on system: %i\n", (active+2));
		h_status = ret;
		
		/*probe for ui*/
		printf("hit the 'any' key if this is the operator...");
		pause(5);
		ret = getchar();
		if (ret == 0) { //FIXME
			ui_flag = 0;
		} else {
			ui_flag = 1;
		}
		ini_flag = 1;
	}
	
	/*normal int/arb operations*/
	if (ui_flag == 1)
		UI();
	
	if (h_status == 1) {
		ret = Master(&local, ui);
		if (ret < 0)
			
