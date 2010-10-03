/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: helloworld.c 19776 2009-01-16 10:34:40Z unhelpful $
 *
 * Copyright (C) 2002 Björn Stenberg
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ****************************************************************************/
/* 
Welcome to the workout plugin.
http://github.com/andrewroth/rockbox_workout/wiki/brainstorming
Andrew Roth
*/

#include "stdio.h"
#include "plugin.h"
#include "string.h"
#include "lib/xlcd.h"

PLUGIN_HEADER

#define STR_LEN          256
#define TXT_LEN          1024
#define MAX_WORKOUTS     48

#define	SCREEN_WIDTH		176
#define	SCREEN_HEIGHT		220
#define	WORKOUT_MENU_WIDTH	152
#define	WORKOUT_MENU_ROW_HEIGHT	12
#define	WORKOUT_MENU_ROWS	12
#define	WORKOUT_MENU_HEIGHT	144 // row_height * rows

#define	MENU_MORE_WIDTH		12
#define	MENU_MORE_HEIGHT	12
#define	MENU_MORE_PADDING	3

/* workout structures */
typedef struct {
	char name[STR_LEN]; 
	long id;
	long created_at;
	long updated_at;
} workout;

typedef struct {
	long when;
	long workout_id;
	long created_at;
	long updated_at;
} workout_date;

/* method stubs */
void setup_fake_data();
void draw_workout_menu();
int read_csv();
void draw_menu_more(int mid_x, int mid_y, bool filled, bool down);

/* data allocations */
workout workouts[MAX_WORKOUTS];
int num_workouts;

enum plugin_status plugin_start(const void* parameter) {
	(void)parameter;
	int button;

	setup_fake_data();

	rb->lcd_set_drawmode(DRMODE_SOLID);
	rb->lcd_set_foreground(LCD_RGBPACK(255, 0, 0));
	rb->lcd_drawpixel(10,10);
	rb->lcd_putsxy(100, 100, "HAI!");

	rb->lcd_update();

	while (1) {
		draw_workout_menu();
		rb->lcd_update();

		button = rb->button_get(true);
		switch (button) {
			case BUTTON_POWER:
				return PLUGIN_OK;
		}
	}
}

void setup_fake_data() {
	strcpy(workouts[0].name, "Upper Body 1");
	workouts[0].id = 1;
	strcpy(workouts[1].name, "Legs 1");
	workouts[1].id = 1;
	strcpy(workouts[2].name, "Stretches");
	workouts[2].id = 1;
	num_workouts = 3;
}

void draw_workout_menu() {
	int i, x, y, half_row;

	x = (SCREEN_WIDTH - WORKOUT_MENU_WIDTH) / 2;
	y = (SCREEN_HEIGHT - WORKOUT_MENU_HEIGHT) / 2;
	half_row = WORKOUT_MENU_ROW_HEIGHT / 2;
	/* border */
	rb->lcd_drawrect(x, y, WORKOUT_MENU_WIDTH, WORKOUT_MENU_HEIGHT); 
	rb->lcd_drawrect(x, y, WORKOUT_MENU_WIDTH, WORKOUT_MENU_ROW_HEIGHT); 
	rb->lcd_drawrect(x, y + WORKOUT_MENU_HEIGHT - WORKOUT_MENU_ROW_HEIGHT, 
			WORKOUT_MENU_WIDTH, WORKOUT_MENU_ROW_HEIGHT); 
	/* more triangles */
	draw_menu_more(SCREEN_WIDTH / 2, y, false, false); 
	draw_menu_more(SCREEN_WIDTH / 2, y + WORKOUT_MENU_HEIGHT - WORKOUT_MENU_ROW_HEIGHT, true, true); 
	/* menu */
	y += WORKOUT_MENU_ROW_HEIGHT + half_row;
	for (i = 0; i < num_workouts; i++, y += WORKOUT_MENU_ROW_HEIGHT) {
		rb->lcd_putsxy(x + half_row, y, workouts[i].name);
	}
}

void draw_menu_more(int mid_x, int mid_y, bool filled, bool down) {
	int x1, y1, x2, y2, x3, y3, old_y1;

	x1 = mid_x;
	y1 = mid_y + MENU_MORE_PADDING;
	x2 = mid_x + MENU_MORE_WIDTH / 2 - MENU_MORE_PADDING;
	y2 = mid_y + MENU_MORE_HEIGHT - MENU_MORE_PADDING - 1;
	x3 = mid_x - MENU_MORE_WIDTH / 2 + MENU_MORE_PADDING;
	y3 = y2;

	if (down) {
		old_y1 = y1;
		y1 = y2;
		y2 = old_y1;
		y3 = old_y1;
	}

	rb->lcd_drawline(x1, y1, x2, y2);
	rb->lcd_drawline(x1, y1, x3, y3);
	rb->lcd_drawline(x2, y2, x3, y3);

	if (filled) {
		xlcd_filltriangle(x1, y1, x2, y2, x3, y3);
	}
}

int read_csv(char *name) {
	char line[STR_LEN], cname[STR_LEN], type[STR_LEN], value[STR_LEN];
	FILE *f;
	unsigned int i, last, section;

	f = fopen(name, "r");
	while (!feof(f)) {
		fgets(line, 256, f);
		printf(line);

		// split by commas
		section = last = 0;
		for (i = 0; i < strlen(line); i++) {
			if (line[i] == ',' || i == strlen(line) - 1) {
				if (section == 0) {
					strncpy(cname, line + last, i - last);
					cname[i - last] = 0;
				} else if (section == 1) {
					strncpy(type, line + last, i - last);
					type[i - last] = 0;
				} else if (section == 2) {
					strncpy(value, line + last, i - last);
					value[i - last] = 0;
				}
				section += 1;
				last = i + 1;
			}
		}

		printf("name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	}

	return true;
}

