#dbes_alpha makefile
#origin 2-25-09
#updated 2-25-09
#jrm

srcdir = .

CC = arm-linux-gcc
CFLAGS = -I$(srcdir)

SUBDIRS = int_arb controls sensors

OBJ1 = int_arb/intarb.o int_arb/ui.o int_arb/elections.o int_arb/slave.o\
			 int_arb/master.o int_arb/candriver.o
OBJ2 = sensors/adc_main.o sensors/di.o sensors/sensors.o
OBJ3 = controls/pwm.o controls/do.o controls/hyd_control.o
OBJ4 = main.o
OBJS = $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4)
			 
dbes_alpha :	$(OBJ4) subdirs
				$(CC) -o dbes_alpha $(OBJS)

#cd subdir && $(MAKE)


.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
		$(MAKE) -C $@


				
main.o :	main.c include/control_str.h
			$(CC) -c main.c
			
.PHONY : clean

clean :
		rm dbes_alpha $(OBJS) 
