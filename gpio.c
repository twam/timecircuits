#include "gpio.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int gpio_export(unsigned int gpio)
{
	int ret;
	char *directory;

	ret = asprintf(&directory, "/sys/class/gpio/gpio%u", gpio);
	if (ret < 0)
		return ret;

	if (access(directory, F_OK ) != -1 ) {
		// gpio is already exported
		return 0;
	}

	free(directory);

	int f = open("/sys/class/gpio/export", O_WRONLY);

	if (f < 0) {
		perror("open");
		return f;
	}

	ret = dprintf(f, "%u", gpio);

	close(f);

	return ret < 0 ? ret : 0;
}

int gpio_unexport(unsigned int gpio)
{
	int ret;
	int f = open("/sys/class/gpio/unexport", O_WRONLY);

	if (f < 0) {
		perror("open");
		return f;
	}

	ret = dprintf(f, "%u", gpio);

	close(f);

	return ret < 0 ? ret : 0;
}

int gpio_direction(unsigned int gpio, enum gpio_direction direction)
{
	int ret;
	char *filename;
	int f;

	ret = asprintf(&filename, "/sys/class/gpio/gpio%u/direction", gpio);
	if (ret < 0)
		return ret;

	f = open(filename, O_WRONLY);
	if (f < 0) {
		perror("open");
		return f;
	}

	ret = dprintf(f, direction == GPIO_IN ? "in" : "out");

	close (f);

	free(filename);

	return ret < 0 ? ret : 0;
}

int gpio_out(unsigned int gpio, unsigned int value)
{
	int ret;
	char *filename;
	int f;

	ret = asprintf(&filename, "/sys/class/gpio/gpio%u/value", gpio);
	if (ret < 0)
		return ret;

	f = open(filename, O_WRONLY);
	if (f < 0) {
		perror("open");
		return f;
	}

	ret = dprintf(f, "%u", value);

	close (f);

	free(filename);

	return ret < 0 ? ret : 0;
}