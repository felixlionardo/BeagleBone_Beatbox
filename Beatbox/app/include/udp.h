#ifndef _UDP_H_
#define _UDP_H_

/* Module to initialize and cleanup the udp thread.
The udp thread handles listening for commands over udp.
It has the "stop" command to exit the program gracefully.
*/

#include <stdbool.h>

void createUDPThread();
void joinUDPThread();

#endif