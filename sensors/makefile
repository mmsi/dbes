#sensors makefile
#jrm<jrcowboy79@gmail.com>
#3-18-09

sensors : adc_main.o di.o sensors.o ;

control_str.h : 
        
        
adc_main.o : adc/adc_main.c adc/ep93xx_adc_dbes.h adc/peekpoke_dbes.h\
			control_str.h
		$(CC) -c adc/adc_main.c $(CFLAGS) $(EXTRAFLAGS)

di.o :  di.c control_str.h
		$(CC) -c di.c $(CFLAGS) $(EXTRAFLAGS)

sensors.o : sensors.c control_str.h
		$(CC) -c sensors.c $(CFLAGS) $(EXTRAFLAGS)
