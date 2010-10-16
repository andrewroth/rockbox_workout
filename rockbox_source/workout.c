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
#define MAX_EXERCISES			512
#define MAX_SETS_PER_EXERCISE		12
#define	MAX_EXERCISE_SETS		MAX_EXERCISES*MAX_SETS_PER_EXERCISE
#define	MAX_BUFFERS			2

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
#define	WORKOUT_SET_WIDTH		60
#define	WORKOUT_SET_MARGIN		5
#define	WORKOUT_SETS_PER_ROW		2

/* menu more/less indicators - little triangle that designates more entries in a list */
#define	MENU_MORE_WIDTH		8
#define	MENU_MORE_HEIGHT	8

/* workout structures */
typedef struct {
	char name[STR_LEN];
	long position;
	long exercise_id;
} exercise_set;

typedef struct {
	char name[STR_LEN]; 
	char description[STR_LEN]; 
	long n;
	long exercise_type_id;
	long created_at;
	long updated_at;
	exercise_set *sets[MAX_SETS_PER_EXERCISE];
	int num_sets;
} exercise;

typedef struct {
	char name[STR_LEN]; 
	long id;
	long created_at;
	long updated_at;
	long num_exercises;
	exercise *exercises[MAX_EXERCISES_PER_WORKOUT];
} workout;

/* not sure if this will be necessary */
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
void workout_fwd();
void workout_back();
int read_csv();
void draw_menu_more(int mid_x, int mid_y, bool filled, bool down);
void set_screen_to_workout_menu();
void set_screen_to_workout();
void draw_workout();
void draw_workout_buffer();
void init_debug();
void debug(char *s);
void copy_screen_to_buffer();
void load_csvs();

/* data allocations */
workout workouts[MAX_WORKOUTS];
exercise exercises[MAX_EXERCISES];
exercise_set exercise_sets[MAX_EXERCISE_SETS];
int num_workouts;
int num_exercises;

/* screens */
#define	WORKOUT_MENU	1
#define	WORKOUT		2

/* globals */
char buffers[MAX_BUFFERS][LCD_WIDTH * LCD_HEIGHT * LCD_DEPTH];
int next_free_buffer = 0;
int workout_menu_selected_row = 0;
int workout_menu_top_item_index = 0;
int workout_top_item_index = 0;
workout *curr_workout;
int app_current_screen = WORKOUT_MENU;
int debug_fd;
char debug_line[STR_LEN];
exercise *workout_selected_exercise = NULL;
int workout_selected_exercise_index;
exercise_set *workout_selected_set = NULL;
int workout_selected_set_index;

enum plugin_status plugin_start(const void* parameter) {
	(void)parameter;
	int button;
	bool scroll_fwd, scroll_back;

	init_debug();
	debug("=====================================================");
	debug("Program startup");
	setup_fake_data();
	load_csvs();
	clear_screen();

	/* viewport test */
	//char store[LCD_WIDTH * LCD_HEIGHT * LCD_DEPTH];
	//rb->lcd_putsxy(0, 0, "PUTS");
	//rb->memcpy(store, rb->lcd_framebuffer, LCD_WIDTH * LCD_HEIGHT * LCD_DEPTH);
	/* end viewport test */

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
			if (scroll_fwd) {
				workout_fwd();
			} else if (scroll_back) {
				workout_back();
			}

			draw_workout();
		}

		//rb->memcpy(rb->lcd_framebuffer, store, LCD_WIDTH * LCD_HEIGHT * LCD_DEPTH);
		rb->lcd_update();

		scroll_fwd = scroll_back = false;
		button = rb->button_get(true);
		switch (button) {
			case BUTTON_POWER:
				debug("Program end");
				return PLUGIN_OK;
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
	debug("Program end");
}

void init_debug() {
	debug_fd = rb->open("/workout.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
}

void debug(char *s) {
	if (s[rb->strlen(s)-1] != '\n') {
		rb->fdprintf(debug_fd, "%s\n", s);
	} else {
		rb->fdprintf(debug_fd, "%s", s);
	}
}

void clear_screen() {
	rb->lcd_set_drawmode(DRMODE_SOLID);
	rb->lcd_set_foreground(LCD_RED);
	rb->lcd_set_background(LCD_BLACK);
	rb->lcd_clear_display();
}

void set_screen_to_workout_menu() {
	debug("Screen switch to workout menu");
	app_current_screen = WORKOUT_MENU;
	clear_screen();
}

void set_screen_to_workout() {
	debug("Screen switch to workout view");
	if (app_current_screen == WORKOUT_MENU) {
		curr_workout = workouts + workout_menu_selected_row;
		if (curr_workout->num_exercises > 0) {
			workout_selected_exercise = curr_workout->exercises[0];
			workout_selected_exercise_index = 0;
		}
	}
	app_current_screen = WORKOUT;
	clear_screen();
}

void setup_fake_data() {
	/* sets */
	exercise_sets[0].exercise_id = 0;
	rb->strcpy(exercise_sets[0].name, "A1");
	exercise_sets[0].position = 0;
	exercise_sets[1].exercise_id = 0;
	rb->strcpy(exercise_sets[1].name, "A2");
	exercise_sets[1].position = 1;
	exercise_sets[2].exercise_id = 0;
	rb->strcpy(exercise_sets[2].name, "A3");
	exercise_sets[2].position = 2;

	/* exercises */
	num_exercises = 3;
	rb->strcpy(exercises[0].name, "ExA");
	exercises[0].sets[0] = exercise_sets + 0;
	exercises[0].sets[1] = exercise_sets + 1;
	exercises[0].sets[2] = exercise_sets + 2;
	exercises[0].num_sets = 3;
	rb->strcpy(exercises[1].name, "ExB");
	rb->strcpy(exercises[2].name, "ExC");
	
	/* workouts */
	rb->strcpy(workouts[0].name, "Upper Body 1");
	workouts[0].id = 0;
	workouts[0].num_exercises = 0;
	rb->strcpy(workouts[1].name, "Legs 1");
	workouts[1].id = 1;
	workouts[1].num_exercises = 0;
	rb->strcpy(workouts[2].name, "Stretches");
	workouts[2].id = 2;
	workouts[2].num_exercises = 0;
	rb->strcpy(workouts[3].name, "W4");
	workouts[3].id = 3;
	workouts[3].num_exercises = 0;

	rb->strcpy(workouts[4].name, "W5");
	workouts[4].id = 4;
	workouts[4].num_exercises = 3;
	workouts[4].exercises[0] = exercises;
	workouts[4].exercises[1] = exercises + 1;
	workouts[4].exercises[2] = exercises + 2;

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

/* tries to move forward one set.  return true if successful, false if at the last set of the current set */
bool workout_fwd_set() {
	if (workout_selected_set_index < workout_selected_exercise->num_sets - 1) {
		workout_selected_set++;
		workout_selected_set_index++;
		return true;
	} else {
		return false;
	}
}

/* tries to move forward one exercise.  returns true if successful, false if at the last exercise */
bool workout_fwd_exercise() {
	if (workout_selected_exercise_index < curr_workout->num_exercises - 1) {
		workout_selected_exercise_index++;
		workout_selected_exercise++;
		return true;
	} else {
		return false;
	}
}

/* tries to move backward one set.  return true if successful, false if at the last set of the current set */
bool workout_back_set() {
	if (workout_selected_set_index > 0) {
		workout_selected_set--;
		workout_selected_set_index--;
		return true;
	} else {
		return false;
	}
}

/* tries to move backward one exercise.  returns true if successful, false if at the last exercise */
bool workout_back_exercise() {
	if (workout_selected_exercise_index > 0) {
		workout_selected_exercise_index--;
		workout_selected_exercise--;
		return true;
	} else {
		return false;
	}
}

void workout_fwd() {
	/* if workout_selected_exercise isn't set by this point, we can safely assume there are no exercises */
	if (workout_selected_exercise == NULL) {
		return;
	}

	/* if no set is chosen, go to the first set, or go on to the next exercise if no sets */
	if (workout_selected_exercise != NULL && workout_selected_set == NULL) {
		if (workout_selected_exercise->num_sets > 0) {
			workout_selected_set = workout_selected_exercise->sets[0];
			workout_selected_set_index = 0;
		} else if (workout_fwd_exercise()) {
			workout_fwd();
		}
		return;
	}

	/* otherwise move the set forward one or jump to the next exercise. */
	if (!workout_fwd_set() && workout_fwd_exercise()) {
		workout_selected_set = NULL;
	}
}

void workout_back() {
	int ns;

	/* if workout_selected_exercise isn't set by this point, we can safely assume there are no exercises */
	if (workout_selected_exercise == NULL) {
		return;
	}

	/* if no set is chosen but there is an exercise chosen, go backwards to the last set of the next exercise */
	if (workout_selected_exercise != NULL && workout_selected_set == NULL) {
		if (workout_back_exercise()) {
			ns = workout_selected_exercise->num_sets;
			workout_selected_set = workout_selected_exercise->sets[ns-1];
			workout_selected_set_index = ns-1;
		}
		return;
	}

	/* otherwise move the set backward one, or jump to the exercise only */
	if (!workout_back_set()) {
		workout_selected_set = NULL;
	}
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
	draw_workout_buffer();
}

void draw_workout_buffer() {
	int row, i, j, x, y, c, half_row, title_width;
	bool at_top, at_bottom;
	char set_line[STR_LEN];
	exercise* curr_exercise;
	exercise_set *curr_set;
	
	/* reset buffers, they will be rendered again from here */
	next_free_buffer = 0;

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
	sprintf(debug_line, "In workout view render.  Current workout: %s; num exc: %ld", curr_workout->name, curr_workout->num_exercises);
	debug(debug_line);
	row = 0;
	for (i = 0; i < curr_workout->num_exercises; i++, row++, y += WORKOUT_ROW_HEIGHT) {
		curr_exercise = curr_workout->exercises[i + workout_top_item_index];
		debug("In workout view render loop.");
		// clear out the row with a solid color first
		rb->lcd_set_foreground(BACKGROUND_COLOR);
		rb->lcd_fillrect(0, y, SCREEN_WIDTH, WORKOUT_ROW_HEIGHT);

		/* exercise text */
		rb->lcd_set_foreground(WORKOUT_COLOR);
		rb->lcd_putsxy(WORKOUT_MARGIN + 1, y, curr_exercise->name);

		/* draw box around it if selected */
		if (workout_selected_set == NULL && workout_selected_exercise == curr_exercise) {
			rb->lcd_drawrect(WORKOUT_MARGIN, y, SCREEN_WIDTH - WORKOUT_MARGIN * 2, WORKOUT_ROW_HEIGHT - 1);
		}

		/* exercise sets */
		row++;
		y += WORKOUT_ROW_HEIGHT;
		c = 0;
		for (j = 0; j < curr_workout->exercises[i + workout_top_item_index]->num_sets; j++) {
			curr_set = curr_workout->exercises[i + workout_top_item_index]->sets[j];
			x = c * WORKOUT_SET_WIDTH + WORKOUT_MARGIN + WORKOUT_SET_MARGIN;

			/* clear out space */
			rb->lcd_set_foreground(BACKGROUND_COLOR);
			rb->lcd_fillrect(x, y, WORKOUT_SET_WIDTH, WORKOUT_ROW_HEIGHT);

			/* draw set */
			rb->lcd_set_foreground(WORKOUT_COLOR);
			sprintf(set_line, "%s %dx%d", curr_set->name, 0, 0);
			rb->lcd_putsxy(x + 1, y + 1, set_line);

			/* draw box around it if selected */
			if (workout_selected_set == curr_set) {
				sprintf(set_line, "   select set %s", curr_set->name);
				debug(set_line);
				rb->lcd_drawrect(x, y, WORKOUT_SET_WIDTH, WORKOUT_ROW_HEIGHT - 1);
			}

			/* increment set column */
			c++;
			if (c == WORKOUT_SETS_PER_ROW) {
				row++;
				y += WORKOUT_ROW_HEIGHT;
				c = 0;
			}

		}

		if (row == WORKOUT_ROWS) {
			copy_screen_to_buffer();
			row = 0;
			y = 0;
		}
	}
	copy_screen_to_buffer();
}

void copy_screen_to_buffer() {
	if (next_free_buffer == MAX_BUFFERS) {
		debug("ERROR: out of buffers!");
	} else {
		rb->memcpy(buffers[next_free_buffer], 
				rb->lcd_framebuffer, LCD_WIDTH * LCD_HEIGHT * LCD_DEPTH);
		next_free_buffer++;
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

void *workout_loaded(char cname[STR_LEN], char type[STR_LEN], char value[STR_LEN]) {
	sprintf(debug_line, "name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	debug(debug_line);
}

void load_csvs() {
	load_workouts();
}

void load_workouts() {
	read_csv("/workout/workouts.csv", workout_loaded);
}

int read_csv(char *name, void (*callback)(char[STR_LEN], char[STR_LEN], char[STR_LEN])) {
	char line[STR_LEN], cname[STR_LEN], type[STR_LEN], value[STR_LEN];
	unsigned int i, last, section;
	int fd;

	sprintf(debug_line, "read_csv START: %s", name);
	debug(debug_line);
	fd = rb->open("/workout/workouts.csv", O_RDONLY);
	debug(debug_line);

	while (rb->read_line(fd, line, 256) > 0) {
		debug(line);

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

		//printf("name: {%s} type: {%s} value: {%s}\n", cname, type, value);
		(*callback)(cname, type, value);
	}

	rb->close(fd)
	sprintf(debug_line, "read_csv END: %s", name);
	debug(debug_line);

	return true;
}

