#sensors makefile
#jrm<jrcowboy79@gmail.com>
#3-18-09

CC = gcc
CFLAGS = -I$(sd)
sd = /home/joel/dbes/dbes_src/dbes/

sensors : adc.o di.o sensors.o

adc.o : adc/adc_main.c adc/ep93xx_adc_dbes.h adc/peekpoke_dbes.h\
			$(sd)include/control_str.h
		$(CC) -c adc/adc_main.c $(CFLAGS)include/
		
di.o :  di.c $(sd)include/control_str.h
		$(CC) -c di.c $(CFLAGS)include/
		
sensors.o : sensors.c $(sd)include/control_str.h
			$(CC) -c sensors.c $(CFLAGS)include/
			
