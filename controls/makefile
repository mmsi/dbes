#hyd_control makefile
#jrm<jrcowboy79@gmail.com>
#3-18-09

CC = gcc
CFLAGS = -I$(sd)

sd = /home/joel/dbes/dbes_src/dbes/

controls : pwm.o do.o hyd_control.o

pwm.o : pwm.c $(sd)include/control_str.h
		$(CC) -c pwm.c $(CFLAGS)include/
		
do.o :  do.c include/control.h $(sd)include/control_str.h
		$(CC) -c do.c $(CFLAGS)include/
		
hyd_control.o : hyd_control.c include/control.h $(sd)include/control_str.h
				$(CC) -c hyd_control.c $(CLAGS)include/