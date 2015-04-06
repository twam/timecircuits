#ifndef _GPIO_H
#define _GPIO_H

enum gpio_direction {
	GPIO_IN,
	GPIO_OUT
};

int gpio_export(unsigned int gpio);
int gpio_unexport(unsigned int gpio);
int gpio_direction(unsigned int gpio, enum gpio_direction direction);
int gpio_out(unsigned int gpio, unsigned int value);

#endif
