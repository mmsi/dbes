/**
 *int_arb/intarb.h
 *2-16-09
 *jrm
 *header file for Interface/Arbitor segment
 */

#include"../include/control_str.h"

#define MAX_ADDRESSABLE 255

/*external control information*/
extern struct cnt_template_t control;

/*external ui structure*/
extern struct cnt_template_t ui;

/*external status information array*/
extern struct status_t status_table[];

/*jack addressing*/
extern int active;
extern int jack_lookup_table[];

/*local ui 1=local, 0=remote*/
extern int ui_flag;

/*Int/Arb initialization*/
extern int ini_flag; //FIXME why is this extern?
