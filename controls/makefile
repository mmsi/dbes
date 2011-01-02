#hyd_control makefile
#jrm<jrcowboy79@gmail.com>
#3-18-09

controls : pwm.o do.o hyd_control.o

control_str.h :
        

pwm.o : pwm.c control_str.h
		$(CC) -c pwm.c $(CFLAGS) $(EXTRAFLAGS)
		
do.o :  do.c include/control.h control_str.h
		$(CC) -c do.c $(CFLAGS) $(EXTRAFLAGS)
		
hyd_control.o : hyd_control.c include/control.h control_str.h
				$(CC) -c hyd_control.c $(CFLAGS) $(EXTRAFLAGS)
