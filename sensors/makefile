#sensors makefile
#jrm<jrcowboy79@gmail.com>
#3-18-09

CC = arm-linux-gcc
CFLAGS = -I$(sd)
sd = /home/dbes/

sensors : adc_main.o di.o sensors.o
			
adc_main.o : adc/adc_main.c adc/ep93xx_adc_dbes.h adc/peekpoke_dbes.h\
			$(sd)include/control_str.h
		$(CC) -c adc/adc_main.c $(CFLAGS)include/
		
di.o :  di.c $(sd)include/control_str.h
		$(CC) -c di.c $(CFLAGS)include/
		
sensors.o : sensors.c $(sd)include/control_str.h
			$(CC) -c sensors.c $(CFLAGS)include/
			
