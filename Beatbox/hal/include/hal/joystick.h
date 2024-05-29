#ifndef JOYSTICK_H
#define JOYSTICK_H

/* Module to initialize and cleanup the joystick thread.

*/

enum JoystickDirection {
    NO_DIRECTION,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    CLICK
};

void initializeJoystick();
void createJoystickThread();
void shutdownJoystick();
void joinJoystickThread();

#endif