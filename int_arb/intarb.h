/**
 *intarb.h
 *2-16-09
 *jrm
 *header file for Interface/Arbitor segment
 */

#define MAX_ADDRESSABLE 255

/*main control structure type*/
struct cnt_template_t {
	unsigned short function;
	unsigned short dest;
	unsigned char rate;
};

/*external control information*/
extern struct cnt_template_t control;

/*individual jack status type*/
struct status {
	unsigned char function;
	unsigned short elevation;
	unsigned short pressure;
};

/*external status information array*/
extern struct status status_table[MAX_ADDRESSABLE];

/*jack addressing*/
extern int active;
extern int jack_lookup_table[active]; //FIXME is this legal?

/*local ui 1=local, 0=remote*/
extern int ui_flag;

/*Int/Arb initialization*/
extern int ini_flag;

/*master/slave status*/
