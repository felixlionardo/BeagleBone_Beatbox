#include "hal/audio_mixer.h"
#include "hal/beatbox.h"
#include "hal/gpio.h"
#include "hal/joystick.h"
#include "hal/general_command.h"

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>

#define NUM_JOYSTICK_DIRECTIONS 5

#define JOYSTICK_UP "/sys/class/gpio/gpio26/value"
#define JOYSTICK_RIGHT "/sys/class/gpio/gpio47/value"
#define JOYSTICK_DOWN "/sys/class/gpio/gpio46/value"
#define JOYSTICK_LEFT "/sys/class/gpio/gpio65/value"
#define JOYSTICK_CLICK "/sys/class/gpio/gpio27/value"

#define JOYSTICK_UP_GPIO "26"
#define JOYSTICK_RIGHT_GPIO "47"
#define JOYSTICK_DOWN_GPIO "46"
#define JOYSTICK_LEFT_GPIO "65"
#define JOYSTICK_CLICK_GPIO "27"

#define JOYSTICK_UP_PIN "p8.14"
#define JOYSTICK_RIGHT_PIN "p8.15"
#define JOYSTICK_DOWN_PIN "p8.16"
#define JOYSTICK_LEFT_PIN "p8.18"
#define JOYSTICK_CLICK_PIN "p8.17"

static bool shutdown = false;
static int joystickReading = 0;
static pthread_t joystickThread;

enum JoystickDirection get_joystick_direction()
{
    // Check each joystick direction pin and return the corresponding direction
    if (read_gpio_value(JOYSTICK_UP) == 0)
    {
        return UP;
    }
    else if (read_gpio_value(JOYSTICK_DOWN) == 0)
    {
        return DOWN;
    }
    else if (read_gpio_value(JOYSTICK_LEFT) == 0)
    {
        return LEFT;
    }
    else if (read_gpio_value(JOYSTICK_RIGHT) == 0)
    {
        return RIGHT;
    }
    else if (read_gpio_value(JOYSTICK_CLICK) == 0)
    {
        return CLICK;
    }
    else
    {
        return NO_DIRECTION;
    }
}

// Initialize: config five joystick pins to be used as gpio
void initialize_joystick_config_pin()
{
    const char *gpio_pins[] = {JOYSTICK_LEFT_PIN, JOYSTICK_RIGHT_PIN, JOYSTICK_DOWN_PIN, JOYSTICK_LEFT_PIN, JOYSTICK_CLICK_PIN};

    for (int i = 0; i < NUM_JOYSTICK_DIRECTIONS; ++i)
    {
        config_pin_to_gpio(gpio_pins[i]);
    }
}

// Initialize GPIO pins for joystick by exporting
void initialize_joystick_gpio_export()
{
    const char *gpio_nums[] = {JOYSTICK_UP_GPIO, JOYSTICK_RIGHT_GPIO,
                               JOYSTICK_DOWN_GPIO, JOYSTICK_LEFT_GPIO, JOYSTICK_CLICK_GPIO};

    for (int i = 0; i < NUM_JOYSTICK_DIRECTIONS; ++i)
    {
        // Export the GPIO pin
        export_gpio(gpio_nums[i]);
    }
}

// Initialize direction of GPIO pins for joystick
void initialize_joystick_gpio_direction()
{
    const char *gpio_nums[] = {JOYSTICK_UP_GPIO, JOYSTICK_RIGHT_GPIO,
                               JOYSTICK_DOWN_GPIO, JOYSTICK_LEFT_GPIO, JOYSTICK_CLICK_GPIO};

    for (int i = 0; i < NUM_JOYSTICK_DIRECTIONS; ++i)
    {
        // Configure the GPIO pin as an input
        set_gpio_direction(gpio_nums[i], "in");
    }
}

void initializeJoystick()
{
    initialize_joystick_config_pin();
    initialize_joystick_gpio_export();
    initialize_joystick_gpio_direction();
}

const char *joystickFiles[NUM_JOYSTICK_DIRECTIONS] = {
    JOYSTICK_UP,
    JOYSTICK_RIGHT,
    JOYSTICK_DOWN,
    JOYSTICK_LEFT,
    JOYSTICK_CLICK};

void *updateJoystickReading(void *args)
{
    (void)args;
    enum JoystickDirection lastDirection = NO_DIRECTION;
    int holdCounter = 1; // Counter to track the duration of holding a direction
    bool initialDelay;

    while (!shutdown)
    {
        enum JoystickDirection direction = get_joystick_direction();
        initialDelay = true;

        if (direction != lastDirection)
        {
            // Reset the hold counter if the direction changes
            holdCounter = 1;
        }

        while (direction == UP)
        {
            direction = get_joystick_direction();

            if (initialDelay)
            {

                AudioMixer_incrementVolume();
                joystickReading = 1;

                sleep(0.75);
                initialDelay = false;
            }
            if (holdCounter % 40 == 0)
            {
                AudioMixer_incrementVolume();
            }
            holdCounter++;

            sleepForMs(5);
        }

        while (direction == RIGHT)
        {
            direction = get_joystick_direction();

            if (initialDelay)
            {
                Beatbox_incrementBpm();
                joystickReading = 2;

                sleep(0.75);
                initialDelay = false;
            }
            if (holdCounter % 15 == 0)
            {
                Beatbox_incrementBpm();
            }
            holdCounter++;

            sleepForMs(5);
        }

        while (direction == DOWN)
        {
            direction = get_joystick_direction();

            if (initialDelay)
            {
                AudioMixer_decrementVolume();

                joystickReading = 3;

                sleep(0.75);
                initialDelay = false;
            }
            if (holdCounter % 25 == 0)
            {
                AudioMixer_decrementVolume();
            }
            holdCounter++;

            sleepForMs(5);
        }

        while (direction == LEFT)
        {
            direction = get_joystick_direction();

            if (initialDelay)
            {
                Beatbox_decrementBpm();

                joystickReading = 4;

                sleep(0.75);
                initialDelay = false;
            }
            if (holdCounter % 20 == 0)
            {
                Beatbox_decrementBpm();
            }
            holdCounter++;

            sleepForMs(5);
        }

        while (direction == CLICK)
        {
            direction = get_joystick_direction();

            if (initialDelay)
            {
                Beatbox_incrementPattern();
                joystickReading = 5;

                sleep(0.75);
                initialDelay = false;
            }
            if (holdCounter % 20 == 0)
            {
                Beatbox_incrementPattern();
            }
            holdCounter++;

            sleepForMs(5);
        }

        // polling every 10ms for input
        sleepForMs(10);
    }

    return NULL;
}

int getJoystickReading()
{
    return joystickReading;
}

void createJoystickThread()
{
    pthread_create(&joystickThread, NULL, updateJoystickReading, NULL);
}

void joinJoystickThread()
{
    pthread_join(joystickThread, NULL);
}

void shutdownJoystick()
{
    shutdown = true;
}