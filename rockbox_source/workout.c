/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: workout.c
 *
 * Copyright (C) 2002 Andrew Roth
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

#define STR_LEN          		256
#define TXT_LEN          		1024
#define MAX_WORKOUTS     		48
#define MAX_EXERCISES_PER_WORKOUT	48

/* colors */
#define	LCD_RED			LCD_RGBPACK(255, 0, 0)
#define	LCD_BLUE		LCD_RGBPACK(0, 0, 255)
#define	BACKGROUND_COLOR	LCD_BLACK

/* choose a workout menu defines */
#define	SCREEN_WIDTH		176
#define	SCREEN_HEIGHT		220
#define	WORKOUT_MENU_WIDTH	152
#define	WORKOUT_MENU_ROW_HEIGHT	16
#define	WORKOUT_MENU_ROWS	8
#define	WORKOUT_MENU_HEIGHT	(WORKOUT_MENU_ROW_HEIGHT * (WORKOUT_MENU_ROWS + 3))
#define	WORKOUT_MENU_COLOR	LCD_RED
#define	WORKOUT_MENU_SEL_COLOR	LCD_BLUE

/* workout menu itself */
#define WORKOUT_COLOR			LCD_RED
#define	WORKOUT_MARGIN			5
#define	WORKOUT_ROW_HEIGHT		16
#define	WORKOUT_TITLE_CHAR_WIDTH	8
#define	WORKOUT_ROWS			5

/* menu more/less indicators - little triangle that designates more entries in a list */
#define	MENU_MORE_WIDTH		8
#define	MENU_MORE_HEIGHT	8

/* workout structures */
typedef struct {
	char name[STR_LEN]; 
	char description[STR_LEN]; 
	long n;
	long exercise_type_id;
	long created_at;
	long updated_at;
} exercise;

typedef struct {
	char name[STR_LEN]; 
	long id;
	long created_at;
	long updated_at;
	long num_exercises;
	exercise *exercises[MAX_EXERCISES_PER_WORKOUT];
} workout;

typedef struct {
	long workout_id;
	long exercise_id;
} workout_exercise;

typedef struct {
	long when;
	long workout_id;
	long created_at;
	long updated_at;
} workout_date;

/* method stubs */
void clear_screen();
void setup_fake_data();
void draw_workout_menu();
void workout_menu_fwd();
void workout_menu_back();
int read_csv();
void draw_menu_more(int mid_x, int mid_y, bool filled, bool down);
void set_screen_to_workout_menu();
void set_screen_to_workout();
void draw_workout();

/* data allocations */
workout workouts[MAX_WORKOUTS];
int num_workouts;

/* screens */
#define	WORKOUT_MENU	1
#define	WORKOUT		2

/* globals */
int workout_menu_selected_row = 0;
int workout_menu_top_item_index = 0;
int workout_top_item_index = 0;
workout *curr_workout;
int app_current_screen = WORKOUT_MENU;

enum plugin_status plugin_start(const void* parameter) {
	(void)parameter;
	int button;
	bool scroll_fwd, scroll_back;

	setup_fake_data();
	clear_screen();

	scroll_fwd = scroll_back = false;
	while (1) {
		/* process buttons and draw current screen */
		if (app_current_screen == WORKOUT_MENU) {
			if (scroll_fwd) {
				workout_menu_fwd();
			} else if (scroll_back) {
				workout_menu_back();
			}

			draw_workout_menu();
		} else if (app_current_screen == WORKOUT) {
			/*
			if (scroll_fwd) {
				workout_menu_fwd();
			} else if (scroll_back) {
				workout_menu_back();
			}
			*/

			draw_workout();
		}

		rb->lcd_update();

		scroll_fwd = scroll_back = false;
		button = rb->button_get(true);
		switch (button) {
			case BUTTON_POWER:
				return PLUGIN_OK;
				break;
			case BUTTON_SELECT:
				if (app_current_screen == WORKOUT_MENU) {
					set_screen_to_workout();
				}
				break;
			case BUTTON_SCROLL_FWD:
			case BUTTON_SCROLL_FWD|BUTTON_REPEAT:
				scroll_fwd = true;
				break;
			case BUTTON_SCROLL_BACK:
			case BUTTON_SCROLL_BACK|BUTTON_REPEAT:
				scroll_back = true;
				break;
			case BUTTON_LEFT:
				if (app_current_screen == WORKOUT) {
					set_screen_to_workout_menu();
				}
		}

	}
}

void clear_screen() {
	rb->lcd_set_drawmode(DRMODE_SOLID);
	rb->lcd_set_foreground(LCD_RED);
	rb->lcd_set_background(LCD_BLACK);
	rb->lcd_clear_display();
}

void set_screen_to_workout_menu() {
	app_current_screen = WORKOUT_MENU;
	clear_screen();
}

void set_screen_to_workout() {
	if (app_current_screen == WORKOUT_MENU) {
		curr_workout = workouts + workout_menu_selected_row;
	}
	app_current_screen = WORKOUT;
	clear_screen();
}

void setup_fake_data() {
	rb->strcpy(workouts[0].name, "Upper Body 1");
	workouts[0].id = 0;
	workouts[0].num_exercises = 0;
	rb->strcpy(workouts[1].name, "Legs 1");
	workouts[1].id = 1;
	workouts[0].num_exercises = 0;
	rb->strcpy(workouts[2].name, "Stretches");
	workouts[2].id = 2;
	workouts[0].num_exercises = 0;
	rb->strcpy(workouts[3].name, "W4");
	workouts[3].id = 3;
	workouts[0].num_exercises = 0;
	rb->strcpy(workouts[4].name, "W5");
	workouts[4].id = 4;
	workouts[0].num_exercises = 0;

	num_workouts = 5;
}

void workout_menu_fwd() {
	if (workout_menu_selected_row < num_workouts - 1) {
		workout_menu_selected_row++;
	}
	workout_menu_top_item_index = MAX(0, workout_menu_selected_row - WORKOUT_MENU_ROWS + 1);
}

void workout_menu_back() {
	if (workout_menu_selected_row > 0) {
		workout_menu_selected_row--;
	}
	workout_menu_top_item_index = MIN(workout_menu_selected_row, workout_menu_top_item_index);
}

void draw_workout_menu() {
	int i, x, y, half_row;
	bool at_top, at_bottom;

	rb->lcd_set_foreground(WORKOUT_MENU_COLOR);
	x = (SCREEN_WIDTH - WORKOUT_MENU_WIDTH) / 2;
	y = (SCREEN_HEIGHT - WORKOUT_MENU_HEIGHT) / 2;
	half_row = WORKOUT_MENU_ROW_HEIGHT / 2;

	/* border */
	rb->lcd_drawrect(x, y, WORKOUT_MENU_WIDTH, WORKOUT_MENU_HEIGHT); 
	rb->lcd_drawrect(x, y, WORKOUT_MENU_WIDTH, WORKOUT_MENU_ROW_HEIGHT); 
	rb->lcd_drawrect(x, y + WORKOUT_MENU_HEIGHT - WORKOUT_MENU_ROW_HEIGHT, 
			WORKOUT_MENU_WIDTH, WORKOUT_MENU_ROW_HEIGHT); 

	/* more scroll up/down triangle indicators */
	if (num_workouts > WORKOUT_MENU_ROWS) {
		at_top = workout_menu_top_item_index == 0;
		at_bottom = workout_menu_top_item_index == num_workouts - WORKOUT_MENU_ROWS;
		draw_menu_more(SCREEN_WIDTH / 2, y + WORKOUT_MENU_ROW_HEIGHT / 2, !at_top, false); 
		draw_menu_more(SCREEN_WIDTH / 2, y + WORKOUT_MENU_HEIGHT - WORKOUT_MENU_ROW_HEIGHT / 2, !at_bottom, true); 
	}

	/* menu */
	y += WORKOUT_MENU_ROW_HEIGHT + half_row;
	for (i = 0; i < WORKOUT_MENU_ROWS && i + workout_menu_top_item_index < num_workouts;
			i++, y += WORKOUT_MENU_ROW_HEIGHT) {
		// clear out the row with a solid color first
		rb->lcd_set_foreground(BACKGROUND_COLOR);
		rb->lcd_fillrect(x + half_row, y, WORKOUT_MENU_WIDTH - half_row*2, WORKOUT_MENU_ROW_HEIGHT);
		// set different color if selected
		if (i + workout_menu_top_item_index == workout_menu_selected_row) {
			rb->lcd_set_foreground(WORKOUT_MENU_SEL_COLOR);
		} else {
			rb->lcd_set_foreground(WORKOUT_MENU_COLOR);
		}
		// row text
		rb->lcd_putsxy(x + half_row, y, workouts[i + workout_menu_top_item_index].name);
	}
}

void draw_workout() {
	int i, x, y, half_row, title_width;
	bool at_top, at_bottom;

	rb->lcd_set_foreground(WORKOUT_COLOR);
	x = WORKOUT_MARGIN;
	y = WORKOUT_MARGIN;
	half_row = WORKOUT_ROW_HEIGHT / 2;

	/* draw the workout title */
	title_width = WORKOUT_TITLE_CHAR_WIDTH * rb->strlen(curr_workout->name);
	rb->lcd_putsxy(x + SCREEN_WIDTH / 2 - title_width / 2, y, curr_workout->name);

	/* more scroll up/down triangle indicators */
	/* TODO this later
	if (num_workouts > WORKOUT_MENU_ROWS) {
		at_top = workout_top_item_index == 0;
		at_bottom = workout_top_item_index == curr_workout->num_exercises - WORKOUT_MENU_ROWS;
		draw_menu_more(SCREEN_WIDTH / 2, y, !at_top, false); 
		draw_menu_more(SCREEN_WIDTH / 2, y + WORKOUT_MENU_HEIGHT - WORKOUT_MENU_ROW_HEIGHT, !at_bottom, true); 
	}
	*/

	/* menu */
	y += WORKOUT_ROW_HEIGHT + half_row;
	for (i = 0; i < WORKOUT_ROWS && i + workout_top_item_index < curr_workout->num_exercises;
			i++, y += WORKOUT_ROW_HEIGHT) {
		// clear out the row with a solid color first
		rb->lcd_set_foreground(BACKGROUND_COLOR);
		rb->lcd_fillrect(0, y, SCREEN_WIDTH, WORKOUT_ROW_HEIGHT);
		// set different color if selected
	//	if (i + workout_top_item_index == workout_menu_selected_row) {
	//		rb->lcd_set_foreground(WORKOUT_MENU_SEL_COLOR);
	//	} else {
//			rb->lcd_set_foreground(WORKOUT_MENU_COLOR);
	//	}
		// row text
//		rb->lcd_putsxy(x + half_row, y, workouts[i + workout_menu_top_item_index].name);
	}
}

void draw_menu_more(int mid_x, int mid_y, bool more, bool down) {
	int x1, y1, x2, y2, x3, y3, old_y1;

	x1 = mid_x;
	y1 = mid_y - MENU_MORE_HEIGHT / 2;
	x2 = mid_x + MENU_MORE_WIDTH / 2;
	y2 = mid_y + MENU_MORE_HEIGHT / 2;
	x3 = mid_x - MENU_MORE_WIDTH / 2;
	y3 = y2;

	if (down) {
		old_y1 = y1;
		y1 = y2;
		y2 = old_y1;
		y3 = old_y1;
	}

	if (!more) {
		rb->lcd_set_foreground(BACKGROUND_COLOR);
		xlcd_filltriangle(x1, y1, x2, y2, x3, y3);
	}

	rb->lcd_set_foreground(WORKOUT_MENU_COLOR);
	/*
	   // changed my mind - no non-filled triangle at all, it's easier to parse quickly
	   // when the triangle is either there or not there
	rb->lcd_drawline(x1, y1, x2, y2);
	rb->lcd_drawline(x1, y1, x3, y3);
	rb->lcd_drawline(x2, y2, x3, y3);
	*/

	if (more) {
		xlcd_filltriangle(x1, y1, x2, y2, x3, y3);
	}
}

int read_csv(char *name) {
	if (name[0] == 'a') {
	}
	return 1;
	/*
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
				section++;
				last = i + 1;
			}
		}

		printf("name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	}

	return true;
	*/
}

