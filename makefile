#dbes
#toplevel makefile
#origin 2-25-09
#updated 7-11-14
#jrm<jrcowboy79@gmail.com>

MAJOR = 1
MINOR = 1
STAGE = 1
REVISION = 2

VERSION = $(MAJOR).$(MINOR).$(STAGE).$(REVISION)
export VERSION

srcdir = $(PRJROOT)

#Tool names
CROSS_COMPILE   			= ${TARGET_PREFIX}-
AS                          = $(CROSS_COMPILE)as
AR                          = $(CROSS_COMPILE)ar
CC                          = $(CROSS_COMPILE)gcc
CPP                       	= $(CC) -E
LD                          = $(CROSS_COMPILE)ld

export AS AR CC CPP LD

#Configuation Settings
 #PWM hardware (choose one)
CPLD_XDIO_PWM = y
EXT_AVR_PWM = n
 #SPI framework (choose one)
24_BITBANG_SPI = y
26_SPIDEV = n

configs-y :=
configs-n :=

configs-$(CPLD_XDIO_PWM) += -DCONFIG_XDIOPWM
configs-$(EXT_AVR_PWM) += -DCONFIG_AVRPWM
configs-$(24_BITBANG_SPI) += -DCONFIG_24SPI
configs-$(26_SPIDEV) += -DCONFIG_26SPIDEV

configs-y :=
configs-n :=

EXTRAFLAGS := $(configs-y)
export EXTRAFLAGS

#Build settings
CFLAGS                  = -I./include -I./int_arb/include -mcpu=arm9 -Wall -g
export CFLAGS
export srcdir


#Installation variables
NAME           = dbes
EXEC_NAME = $(NAME)_$(VERSION)

#Build files
SUBDIRS = int_arb controls sensors

OBJ1 = int_arb/intarb.o int_arb/ui.o int_arb/elections.o int_arb/slave.o\
			 int_arb/master.o int_arb/candriver.o
OBJ2 = sensors/adc_main.o sensors/di.o sensors/sensors.o
OBJ3 = controls/pwm.o controls/do.o controls/hyd_control.o
OBJ4 = main.o
OBJS = $(OBJ1) $(OBJ2) $(OBJ3) $(OBJ4)
			 
dbes_alpha :	$(OBJ4) subdirs
				$(CC) -o $(EXEC_NAME) $(OBJS)

#cd subdir && $(MAKE)


.PHONY: subdirs $(SUBDIRS)

subdirs : $(SUBDIRS)

$(SUBDIRS):
		$(MAKE) -C $@


				
main.o :	main.c include/control_str.h
			$(CC) -c main.c $(CFLAGS) $(EXTRAFLAGS)
			
.PHONY : clean

clean :
		rm $(NAME)_* $(OBJS) 
