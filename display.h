#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <inttypes.h>
#include <stddef.h>
#include <time.h>

#define	CHIPS_PER_MODULE		14
#define CHARACTER_SEGMENTS_PER_MODULE	13
#define MODULES				3
#define CHIPS_TOTAL			(MODULES*CHIPS_PER_MODULE)
#define CHARACTER_SEGMENTS_TOTAL 	(MODULES*CHARACTER_SEGMENTS_PER_MODULE)

#define LED_CHIP			9
#define LED_PM_BIT			8
#define LED_AM_BIT			9
#define LED_SECONDS_UPPER_BIT		10
#define LED_SECONDS_LOWER_BIT		11
#define LED_PM				(1<<LED_PM_BIT)
#define LED_AM				(1<<LED_AM_BIT)
#define LED_SECONDS			((1<<LED_SECONDS_LOWER_BIT) | (1<<LED_SECONDS_UPPER_BIT))

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

#define CHARSET_SIZE 128

extern uint16_t charset[CHARSET_SIZE];

void clear_display_data(uint16_t* display_data);
void string_to_display_data(uint16_t* display_data, size_t offset, const char* buf);
void time_to_display_data(uint16_t* display_data, int display, struct tm* tm, int hour24, int seconds);
void print_display_data(uint16_t* display_data);

#endif