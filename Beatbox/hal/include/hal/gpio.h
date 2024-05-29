#ifndef GPIO_H
#define GPIO_H

/* Module to configure, read, and write gpio pins.

*/

#include <stdbool.h>

void config_pin_to_gpio(const char *gpio_pin);
bool is_gpio_exported(const char *gpio_num);
void export_gpio(const char *gpio_num);
void set_gpio_direction(const char *gpio_num, const char *direction);
int read_gpio_value(const char *path);
void write_gpio_value(const char *gpio_num, const char* value);

#endif