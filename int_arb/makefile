#int_arb makefile
#jrm<jrcowboy79@gmail.com>
#3-18-09

#Make rules
int_arb : intarb.o elections.o slave.o master.o candriver.o ui.o

intarb.h : include/intarb.h /home/joel/mms_source/dbes/include/control_str.h

intarb.o : intarb.c
			$(CC) -c intarb.c $(CFLAGS)
			
elections.o : elections.c include/intarb.h include/candriver.h
				$(CC) -c elections.c $(CFLAGS)
				
slave.o : slave.c include/intarb.h include/candriver.h
			$(CC) -c slave.c $(CFLAGS)
			
master.o : master.c include/intarb.h include/candriver.h
			$(CC) -c master.c $(CFLAGS)

#should i have a candriver.h(canmsg.h) dependency here?

candriver.o : candriver.c include/can.h
				$(CC) -c candriver.c $(CFLAGS)

ui.o : ui.c include/intarb.h
		$(CC) -c ui.c $(CFLAGS)
