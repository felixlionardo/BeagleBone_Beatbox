#ifndef _SHUTDOWN_H_
#define _SHUTDOWN_H_

/* Module to initialize and cleanup all threads.
This uses a shutdown mutex and conditional variable to make the main thread wait
for all devices to shutdown before joining threads.
*/

#include <stdbool.h>

void createThreads();
void joinThreads();
void waitShutdown();
void signalShutdown();

#endif