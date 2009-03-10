#include<unistd.h>

int Main(void)
{
	int i;
	
	devmem = open("dev/mem", O_RDWR|O_SYNC);
	
	while (i == 0) {
		Dout(0, 1);
		sleep(2);
		Dout(0, 0);
		sleep(2);
	}
}
