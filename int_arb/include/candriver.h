/**
 *int_arb/candriver.h
 *2-15-09
 *jrm
 *definitions for functions interacting with Driver()
 *Byte 0: jack address
 *Byte 1: function - bit 0 contingency bit, bits 1,2 lift status
 *Bytes 2,3: 16 bit elevation
 *Byte 4: rate
 *exception - status frames(id 5) bytes 4,5: 16bits pressure 
 */

/*driver control defines*/
#define INI 0
#define TX 1
#define RX 2
#define FILTER 3

/*max number of CAN data bytes*/
#define MSG_LENGTH 6

/*message id*/
#define MASTER_PING 0  //no data
#define CONTROL 1	   //5 bytes
#define BROAD_ID 2	   //byte 0
#define CONT 3		   //no data
#define STATUS_REQ 4   //byte 0
#define STATUS 5	   //6 bytes - note exception
#define UIPASS 6	   //5 bytes
#define PING_REPLY 7   //no data

int Driver(unsigned char, char *, unsigned long *);
