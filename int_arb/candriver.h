/**
 *int_arb/candriver.h
 *2-15-09
 *jrm
 *definitions for functions interating with Driver()
 */

/*driver control defines*/
#define INI 0
#define TX 1
#define RX 2
#define FILTER 3

/*max number of CAN data bytes*/
#define MSG_LENGTH 6

/*message id*/
#define MASTER_PING 0
#define CONTROL 1
#define BROAD_ID 2
#define CONT 3
#define STATUS_REQ 4
#define STATUS 5
#define UIPASS 6
#define PING_REPLY 7
