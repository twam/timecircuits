#include "test.h"
#include "spi.h"
#include "display.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void test_pattern(uint16_t pattern)
{
	size_t chip;

	uint16_t display_data[CHIPS_TOTAL];
	for (chip = 0; chip < CHIPS_TOTAL; ++chip) {
		display_data[chip] = pattern;
	}

	printf("Testing pattern 0x%04X\n", pattern);
	spi_transfer(display_data, CHIPS_TOTAL);
}

void test_chip(size_t chip, uint16_t pattern)
{
	uint16_t display_data[CHIPS_TOTAL];

	memset(display_data, 0, CHIPS_TOTAL*sizeof(uint16_t));
	display_data[chip] = pattern;

	printf("Testing chip %2u (Pattern 0x%04X)\n", chip, pattern);
	spi_transfer(display_data, CHIPS_TOTAL);
}

void test_run(char* param)
{
	if (param == NULL) {
		printf(
			"Support test modes:\n"
			"  on                    Switch everything on\n"
			"  off                   Switch everything off\n"
			"  pattern-<PATTERN>     Display <PATTERN> on all chips\n"
			"  chip-<CHIP>           Display pattern 0xFFFF on chip <CHIP>\n"
			"  loop-pattern          Loop through all bits in the pattern\n"
			"  loop-chip             Loop through all chips with pattern 0xFFFF\n"
			"  ascii-<CHARACTER>     Display <CHARACTER> on all chips\n"
			"  location              Show TOP/MIDDLE/BOTTOM on the appropriate displays\n"
			);
	} else if (strcmp(param, "on") == 0) {
		test_pattern(0xFFFF);
	} else if (strcmp(param, "off") == 0) {
		test_pattern(0x0000);
	} else if (strncmp(param, "pattern-", 8) == 0) {
		test_pattern(strtol(param+8, NULL, 0));
	} else if (strncmp(param, "chip-", 5) == 0) {
		test_chip(strtol(param+5, NULL, 0), 0xFFFF);
	} else if (strcmp(param, "loop-pattern") == 0) {
		uint16_t pattern = 1;
		while (1) {
			test_pattern(pattern);
			pattern <<= 1;
			if (pattern == 0)
				pattern = 1;
			usleep(500e3);
		}
	} else if (strcmp(param, "loop-chip") == 0) {
		size_t chip = 0;
		while (1) {
			test_chip(chip, 0xFFFF);
			chip++;
			if (chip == CHIPS_TOTAL)
				chip = 0;
			usleep(500e3);
		}
	} else if (strncmp(param, "ascii-", 6) == 0) {
		uint16_t pattern = 0;

		if (strlen(param) < 7) {
			fprintf(stderr, "No character given.\n");
		} else if (param[6] >= CHARSET_SIZE) {
			fprintf(stderr, "Character '%c' has an ASCII code %u that is larger than charset size.\n", param[6], param[6]);
		} else {
			pattern = charset[(size_t)param[6]];
			printf("ascii: %u\n", param[6]);
			test_pattern(pattern);
		}
	} else if (strcmp(param, "location") == 0) {
		uint16_t display_data[CHIPS_TOTAL];
		char buf[CHIPS_TOTAL+1];

		clear_display_data(display_data);
		snprintf(buf, CHIPS_TOTAL+1, "TOP       TOPMIDDLE MIDDLEBOTTOM BOTTOM");
		string_to_display_data(display_data, 0*CHARACTER_SEGMENTS_PER_MODULE, buf);
		spi_transfer(display_data, CHIPS_TOTAL);
		// print_display_data(display_data);
	} else {
		fprintf(stderr, "Unknown test mode: '%s'\n", param);
	}
}
