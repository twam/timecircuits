#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "display.h"
#include "options.h"
#include "spi.h"
#include "test.h"
#include "gpio.h"

#define COLOR_RESET "\e[0m"
#define COLOR_BOLDWHITE "\e[1m"
#define COLOR_BOLDRED "\e[1;31m"
#define COLOR_BOLDGREEN "\e[2;31m"
#define COLOR_BOLDYELLOW "\e[3;31m"
#define COLOR_GRAY "\e[0;37m"

// red		destination time
// green	present time
// yellow	last time departed

void usertime_r(const time_t *time, struct tm *result)
{
	if (options_utc) {
		gmtime_r(time, result);
	} else {
		localtime_r(time, result);
	}
}

int main(int argc, char *argv[])
{
	uint16_t display_data[CHIPS_TOTAL];
	int ret = 0;

	char buf[CHIPS_TOTAL+1];
	snprintf(buf, CHIPS_TOTAL+1, "OBEN     OBENMITTE   MITTEUNTEN   UNTEN");

	options_parse(argc, argv);

	if (gpio_export(BLANK_GPIO)) {
		fprintf(stderr, "gpio_export failed!\n");
	}

	usleep(100e3);

	if (gpio_direction(BLANK_GPIO, GPIO_OUT)) {
		fprintf(stderr, "gpio_direction failed!\n");
	}

	if (gpio_out(BLANK_GPIO, 0)) {
		fprintf(stderr, "gpio_out failed!\n");
	}

	spi_init();

	do {
		switch (options_mode) {
			case MODE_DEFAULT: {
				struct tm tm_destination;
				struct tm tm_present;
				struct tm tm_departed;

				int display_destination = 0;
				int display_present = 0;
				int display_departed = 0;

				time_t time_destination = 0;
				time_t time_present = 0;
				time_t time_departed = 0;
				time_t time_now;
				time(&time_now);

				if (options_destination_time_valid) {
					time_destination = options_destination_time;
					if (options_relative)
						time_destination += time_now;
					display_destination = 1;
				}

				if (options_present_time_valid) {
					time_present = options_present_time;
					if (options_relative)
						time_destination += time_now;
					display_present = 1;
				} else {
					time_present = time_now;
					display_present = 1;
				}

				if (options_departed_time_valid) {
					time_departed = options_departed_time;
					if (options_relative)
						time_departed += time_now;
					display_departed = 1;
				}

				usertime_r(&time_destination, &tm_destination);
				usertime_r(&time_present, &tm_present);
				usertime_r(&time_departed, &tm_departed);

				clear_display_data(display_data);

				if (display_destination)
					time_to_display_data(display_data, 0, &tm_destination, options_hour24, options_seconds);
				if (display_present)
					time_to_display_data(display_data, 1, &tm_present, options_hour24, options_seconds);
				if (display_departed)
					time_to_display_data(display_data, 2, &tm_departed, options_hour24, options_seconds);

				spi_transfer(display_data, CHIPS_TOTAL);

			break;
			}

			case MODE_TEST:
				test_run(options_test_param);
			break;
		}
	} while (options_loop);

	spi_exit();

	// if (gpio_unexport(BLANK_GPIO)) {
	// 	fprintf(stderr, "gpio_unexport failed!\n");
	// }

	return ret;
}
