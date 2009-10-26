#int_arb makefile
#jrm<jrcowboy79@gmail.com>
#3-18-09

CC = arm-linux-gcc
CFLAGS = -I$(sd)

sd = /home/dbes/

int_arb : intarb.o elections.o slave.o master.o candriver.o ui.o

intarb.h : intarb.h $(sd)include/control_str.h

intarb.o : intarb.c $(sd)include/control_str.h
			$(CC) -c intarb.c $(CFLAGS)include/
			
elections.o : elections.c include/intarb.h include/candriver.h\
               $(sd)include/unique.h
				$(CC) -c elections.c $(CFLAGS)include/
				
slave.o : slave.c include/intarb.h include/candriver.h $(sd)include/unique.h
			$(CC) -c slave.c $(CFLAGS)include/
			
master.o : master.c include/intarb.h include/candriver.h
			$(CC) -c master.c $(CFLAGS)include/

#should i have a candriver.h(canmsg.h) dependency here?

candriver.o : candriver.c include/can.h
				$(CC) -c candriver.c

ui.o : ui.c include/intarb.h
		$(CC) -c ui.c $(CFLAGS)include/