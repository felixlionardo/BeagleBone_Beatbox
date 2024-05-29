#ifndef _PRINT_RESULT_H_
#define _PRINT_RESULT_H_

/* Module to initialize and cleanup the printing thread.
The printing thread handles printing sampler data from the previous second to the terminal every second.
It shares a lock data_mutex with sampler.c for thread-safe access to the history data.
*/

#include <stdbool.h>

void createPrintingThread();
void joinPrintingThread();
void shutdownPrintingThread();

#endif