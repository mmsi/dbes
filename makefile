#dbes_alpha makefile
#origin 2-25-09
#updated 2-25-09
#jrm

dbes_alpha : intarb.o ui.o elections.o slave.o master.o candriver.o\
			 adc.o di.o sensors.o\
			 pwm.o do.o hydcontrol.o\
			 mainloop.o main.o
			 
intarb.o :  intarb.c intarb.h control_str.h unique.h
			gcc -c intarb.c
			
elections.o :   elections.c intarb.h 