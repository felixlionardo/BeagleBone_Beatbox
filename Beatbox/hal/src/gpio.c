#include "hal/gpio.h"
#include "hal/general_command.h"

#define LONG_COMMAND_SIZE 50
#define READ_SINGLE_VALUE_BUFFER_SIZE 2

void config_pin_to_gpio(const char *gpio_pin)
{
    char configPinCommand[LONG_COMMAND_SIZE];
    snprintf(configPinCommand, sizeof(configPinCommand), "config-pin %s gpio", gpio_pin);
    runCommand(configPinCommand);
}

bool is_gpio_exported(const char *gpio_num) {
    char path[256];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%s", gpio_num);

    FILE *file = fopen(path, "r");
    if (file != NULL) {
        fclose(file);
        return true;  // Directory exists, GPIO is exported
    } else {
        return false;  // Directory does not exist, GPIO is not exported
    }
}

// Do not export if already exported, same for unexport
void export_gpio(const char *gpio_num)
{
    if (!is_gpio_exported(gpio_num))
    {
        char exportCommand[LONG_COMMAND_SIZE];
        snprintf(exportCommand, sizeof(exportCommand), "echo %s > /sys/class/gpio/export", gpio_num);
        runCommand(exportCommand);
    }
}

// Set input("in") or output("out") for gpio
void set_gpio_direction(const char *gpio_num, const char *direction)
{
    char directionCommand[LONG_COMMAND_SIZE];
    snprintf(directionCommand, sizeof(directionCommand), "echo %s > /sys/class/gpio/gpio%s/direction", direction, gpio_num);
    runCommand(directionCommand);
}

// Read and return gpio value 0 or 1
int read_gpio_value(const char *path)
{
    char valueCommand[LONG_COMMAND_SIZE];
    snprintf(valueCommand, sizeof(valueCommand), "cat %s", path);

    FILE *pipe = popen(valueCommand, "r");
    if (pipe == NULL)
    {
        perror("Error opening value file");
        exit(EXIT_FAILURE);
    }

    char buffer[READ_SINGLE_VALUE_BUFFER_SIZE];
    if (fgets(buffer, sizeof(buffer), pipe) == NULL)
    {
        perror("Error reading from value file");
        pclose(pipe);
        exit(EXIT_FAILURE);
    }

    pclose(pipe);

    return atoi(buffer);
}

void write_gpio_value(const char *path, const char *value)
{
    char file[LONG_COMMAND_SIZE];
    snprintf(file, sizeof(file), "%s", path);
    writeToFile(file, value);
}