#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

/* Module to initialize and cleanup the accelerometer thread.

*/

#include <stdint.h>

void initializeAccelerometer();
void createAccelerometerThread();
void shutdownAccelerometer();
void joinAccelerometerThread();

#endif