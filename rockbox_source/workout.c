/***************************************************************************
 *             __________               __   ___.
 *   Open      \______   \ ____   ____ |  | _\_ |__   _______  ___
 *   Source     |       _//  _ \_/ ___\|  |/ /| __ \ /  _ \  \/  /
 *   Jukebox    |    |   (  <_> )  \___|    < | \_\ (  <_> > <  <
 *   Firmware   |____|_  /\____/ \___  >__|_ \|___  /\____/__/\_ \
 *                     \/            \/     \/    \/            \/
 * $Id: workout.c
 *
 * Copyright (C) 2010 Andrew Roth
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
#include "time.h"
#include "lib/xlcd.h"

PLUGIN_HEADER

#if defined(SIMULATOR)
#define	DEBUG_ENABLED			true
#else
#define	DEBUG_ENABLED			false
#endif
#define	debug_print(...)		if (DEBUG_ENABLED) { rb->snprintf(debug_line_ptr, TXT_LEN, __VA_ARGS__); debug(debug_line_ptr); }

#define	init_int(var, val)		var = ((var) != 0 ? (var) : (val))
#define	init(var, val)			var = ((var) != NULL ? (var) : (val))
#define STR_LEN          		48
#define TXT_LEN          		1024
#define MAX_WORKOUTS     		10
#define MAX_WORKOUT_DATES    		30
#define MAX_EXERCISES_PER_WORKOUT	20
#define MAX_EXERCISES			120
#define MAX_SETS_PER_EXERCISE		5
#define	MAX_EXERCISE_SETS		MAX_EXERCISES*MAX_SETS_PER_EXERCISE
//#define	MAX_EXERCISE_SETS		120*5     // 600
#define	MAX_EXERCISE_LOGS		MAX_WORKOUT_DATES*MAX_EXERCISES_PER_WORKOUT
//#define	MAX_EXERCISE_LOGS		30*20     // 600
//#define	MAX_SET_LOGS			MAX_EXERCISE_LOGS*MAX_SETS_PER_EXERCISE
//#define	MAX_SET_LOGS			30*20*5   // 3000
#define	MAX_SET_LOGS			14*MAX_EXERCISES_PER_WORKOUT*MAX_SETS_PER_EXERCISE
#define	MAX_BUFFERS			0
#define	MAX_FUNCTIONS			MAX_SET_LOGS*3 // weight, reps, rest
#define	SAVE_LOGS_SECONDS		60

/* colors */
#define	LCD_RED			LCD_RGBPACK(255, 0, 0)
#define	LCD_BLUE		LCD_RGBPACK(0, 0, 255)
#define	LCD_ORANGE		LCD_RGBPACK(255, 165, 0)
#define	LCD_YELLOW		LCD_RGBPACK(255, 255, 0)
#define	LCD_GREEN		LCD_RGBPACK(0, 255, 0)
#define	LCD_AQUA		LCD_RGBPACK(127,255,212)
#define	BACKGROUND_COLOR	LCD_BLACK

/* choose a workout menu defines */
#define	SCREEN_WIDTH			LCD_WIDTH
#define	SCREEN_HEIGHT			LCD_HEIGHT
#define	WORKOUT_MENU_WIDTH		(LCD_WIDTH - WORKOUT_MENU_MARGIN*2)
//#define	WORKOUT_MENU_HEIGHT		(WORKOUT_MENU_ROW_HEIGHT * WORKOUT_MENU_ROWS) + WORKOUT_MENU_HEADER_HEIGHT*2
#define	WORKOUT_MENU_HEIGHT		(LCD_HEIGHT - WORKOUT_MENU_MARGIN*2)
#define	WORKOUT_MENU_ROW_HEIGHT		32
#define	WORKOUT_MENU_ROWS		4
#define	WORKOUT_MENU_HEADER_HEIGHT	16
#define	WORKOUT_MENU_MARGIN		5
#define	WORKOUT_MENU_COLOR		LCD_RED
#define	WORKOUT_MENU_SEL_COLOR		LCD_BLUE

/* workout menu itself */
#define WORKOUT_COLOR			LCD_RED
#define WORKOUT_SETUP_COLOR		LCD_ORANGE
#define WORKOUT_INPROGRESS_COLOR	LCD_YELLOW
#define WORKOUT_RESTING_COLOR		LCD_AQUA
#define WORKOUT_DONE_COLOR		LCD_GREEN
#define	WORKOUT_MARGIN			5
#define	WORKOUT_ROW_HEIGHT		16
#define	WORKOUT_TITLE_CHAR_WIDTH	8
#define	WORKOUT_ROWS			8
#define	WORKOUT_SET_WIDTH		160
#define	WORKOUT_SET_MARGIN		5
#define	WORKOUT_SETS_PER_ROW		1
#define	WORKOUT_DASHBOARD_PERCENT	28.0
#define	WORKOUT_DASHBOARD_TOP		((100.0-WORKOUT_DASHBOARD_PERCENT)/100.0)*SCREEN_HEIGHT
#define	WORKOUT_DASHBOARD_PROG_HEIGHT	10
#define	WORKOUT_DASHBOARD_PROG_LEFT	30
#define	WORKOUT_DASHBOARD_PROG_TOP	10

/* exercise types */
#define	EXERCISE_TYPE_WEIGHTS		1
#define	EXERCISE_TYPE_CARDIO		2
#define	EXERCISE_TYPE_STRETCH		3

/* exercise states */
#define	NUM_EXERCISE_STATES		4
#define	EXERCISE_SETTING_UP		0
#define	EXERCISE_INPROGRESS		1
#define	EXERCISE_RESTING		2
#define	DONE_WORKOUT			3

/* menu more/less indicators - little triangle that designates more entries in a list */
#define	MENU_MORE_WIDTH		8
#define	MENU_MORE_HEIGHT	8

typedef struct workout_log_entry_s workout_log_entry;
typedef struct workout_date_s workout_date;
typedef struct workout_s workout;
typedef struct exercise_s exercise;

/* workout structures */
typedef struct {
	int id;
	char name[STR_LEN];
	int position;
	exercise *exercise_ref;
	unsigned short int row;
	unsigned short int reps;
	int hold_for;
	int weight;
} exercise_set;

struct workout_s {
	int id;
	char name[STR_LEN]; 
	//time_t created_at;
	//time_t updated_at;
	int num_exercises;
	exercise *exercises[MAX_EXERCISES_PER_WORKOUT];
};

struct exercise_s {
	int id;
	char name[STR_LEN]; 
	char description[STR_LEN]; 
	int n;
	unsigned short int exercise_type_id;
	//time_t created_at;
	//time_t updated_at;
	exercise_set *sets[MAX_SETS_PER_EXERCISE];
	int num_sets;
	unsigned short int row;
};

typedef struct {
	int id;
	int workout_id;
	int exercise_id;
} workout_exercise;

struct workout_date_s {
	int id;
	char when[STR_LEN];
	long when_int;
	workout *workout;
	//time_t created_at;
	//time_t updated_at;
	time_t started_at;
	time_t finished_at;
	int n;
};

typedef struct {
	time_t created_at;
	time_t started_at;
	time_t last_completed_at;
	time_t finished_at;
	workout_date *workout_date;
	exercise *exercise;
} exercise_log_entry;

typedef struct {
	time_t created_at;
	time_t started_at;
	time_t completed_at;
	time_t done_rested_at;
	exercise_set *exercise_set;
	exercise_log_entry *exercise_log_entry;
	int n;
	short int position;
} set_log_entry;

typedef struct {
	int id;
	exercise_set *exercise_set;
	char variable[STR_LEN];
	int base;
	int inc;
	int round_to_nearest;
	int min_n;
	int max_n;
	int min_v;
	int max_v;
} function;

/* method stubs */
void clear_screen(void);
void setup_fake_data(void);
void draw_workout_menu(void);
void workout_menu_fwd(void);
void workout_menu_back(void);
void workout_fwd(void);
void workout_back(void);
int read_csv(char *name, void (*callback)(char[STR_LEN], char[STR_LEN], char[STR_LEN]));
void draw_menu_more(int mid_x, int mid_y, bool filled, bool down);
void set_screen_to_workout_menu(void);
void set_screen_to_workout(void);
void draw_workout(void);
void draw_workout_buffer(void);
void init_debug(void);
void debug(char *s);
void copy_screen_to_buffer(void);
void load_csvs(void);
void load_workouts(void);
long now(void);
void tick(void);
void draw_workout_dashboard(void);
void set_playback_state(int state);
float calculate_function(char variable[STR_LEN], exercise_set *es, long n, float default_val);
void write_workout_dates(void);
void write_exercise_logs(void);
void write_set_log_entries(void);
void save_all_logs(void);
void morse_play_int(int i);
void morse_play(char c);
void morse_dot(void);
void morse_dash(void);

/* data allocations */
workout workouts[MAX_WORKOUTS];
workout_exercise workout_exercises[MAX_WORKOUTS * MAX_EXERCISES];
workout_date workout_dates[MAX_WORKOUT_DATES];
exercise exercises[MAX_EXERCISES];
exercise_set exercise_sets[MAX_EXERCISE_SETS];
exercise_log_entry exercise_log_entries[MAX_EXERCISE_LOGS];
set_log_entry set_log_entries[MAX_SET_LOGS];
function functions[MAX_FUNCTIONS];
int num_workouts = 0;
int num_workout_dates = 0;
int num_exercises = 0;
int num_workout_exercises = 0;
int num_exercise_sets = 0;
int num_workout_log_entries = 0;
int num_exercise_log_entries = 0;
int num_set_log_entries = 0;
int num_functions = 0;

/* finders */
exercise_log_entry *find_exercise_log_entry(workout_date *wd, exercise *e);
exercise_log_entry *find_exercise_log_entry_from_created_at(time_t created_at);
exercise_log_entry *find_or_create_exercise_log_entry(workout_date *wd, exercise *e);
set_log_entry *find_set_log_entry(exercise_log_entry *ele, exercise_set *s);
set_log_entry *find_or_create_set_log_entry(exercise_log_entry *ele, exercise_set *s);
workout *find_workout(long id);
exercise *find_exercise(long id);
workout_date *find_workout_date(long id);
exercise_set *find_exercise_set(int id);

/* screens */
#define	WORKOUT_MENU	1
#define	WORKOUT		2

/* state vars */
int app_current_screen = WORKOUT_MENU;
int playback_state;
time_t playback_last_state_change;
time_t last_save;
bool playback_is_last_set = false;
float playback_stay_seconds;
long int time_since_last_tick;
int state_seconds;
#define	DEFAULT_SECONDS_ON_SETUP_FIRST_EXERCISE		60
int DEFAULT_SECONDS_ON_STATE[NUM_EXERCISE_STATES] = {
	10.0 /* setup */,
	30.0 /* inprogress */,
	30.0 /* rest */,
	0.0 /* done workout */
};
int DEFAULT_SECONDS_ON_STATE_FOR_STRETCH[NUM_EXERCISE_STATES] = {
	5.0 /* setup */,
	30.0 /* inprogress */,
	5.0 /* rest */,
	0.0 /* done workout */
};
char *STATE_STR[NUM_EXERCISE_STATES] = {
	"setup weight" /* setup */,
	"lift!" /* inprogress */,
	"rest" /* rest */,
	"done workout." /* done workout */
};
char *STATE_STR_FOR_STRETCH[NUM_EXERCISE_STATES] = {
	"setup" /* setup */,
	"hold" /* inprogress */,
	"rest" /* rest */,
	"done stretching." /* done workout */
};
char *STATE_WAIT_VAR[NUM_EXERCISE_STATES] = {
	"setup" /* setup */,
	"lift" /* inprogress */,
	"rest" /* rest */,
	"done" /* done workout */
};
char *STATE_WAIT_VAR_FOR_STRETCH[NUM_EXERCISE_STATES] = {
	"setup" /* setup */,
	"hold_for" /* inprogress */,
	"rest" /* rest */,
	"done" /* done workout */
};
int STATE_BEEP_FREQ[NUM_EXERCISE_STATES] = {
	1000 /* setup */,
	2000 /* inprogress */,
	4000 /* rest */,
	8000 /* done workout */
};
int STATE_BEEP_DURATION[NUM_EXERCISE_STATES] = {
	100 /* setup */,
	100 /* inprogress */,
	100 /* rest */,
	100 /* done workout */
};
int STATE_BEEP_AMPLITUDE[NUM_EXERCISE_STATES] = {
	6000 /* setup */,
	6000 /* inprogress */,
	6000 /* rest */,
	6000 /* done workout */
};

/* exercise playback */
exercise *playback_exercise = NULL;
exercise_log_entry *playback_exercise_log = NULL;
int playback_exercise_index;
exercise_set *playback_set = NULL;
int playback_set_index;
set_log_entry *playback_set_log = NULL;

/* globals */
#if MAX_BUFFERS > 0
char buffers[MAX_BUFFERS][LCD_WIDTH * LCD_HEIGHT * LCD_DEPTH];
#endif
int next_free_buffer = 0;
int workout_menu_selected_row = 0;
int workout_menu_top_item_index = 0;
int workout_top_item_index = 0;
workout *curr_workout;
workout_date *curr_workout_date;
int debug_fd;
char debug_line[TXT_LEN];
char *debug_line_ptr = debug_line;
exercise *workout_selected_exercise = NULL;
int workout_selected_exercise_index;
exercise_set *workout_selected_set = NULL;
int workout_selected_set_index;
bool workout_reset_cache = true;
int workout_top_row = 0;

enum plugin_status plugin_start(const void* parameter) {
	(void)parameter;
	int button;
	bool scroll_fwd, scroll_back;

	init_debug();
	debug("=====================================================");
	debug("Program startup");
	debug_print("current time: %ld", now());
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
		button = rb->button_get_w_tmo(25);
		tick();
		switch (button) {
			case BUTTON_POWER:
				debug("Program end");
				return PLUGIN_OK;
			case BUTTON_SELECT:
				if (app_current_screen == WORKOUT_MENU) {
					set_screen_to_workout();
				} else if (app_current_screen == WORKOUT) {
					if (workout_selected_exercise != NULL && workout_selected_set != NULL) {
						playback_exercise = workout_selected_exercise;
						playback_exercise_index = workout_selected_exercise_index;
						playback_set = workout_selected_set;
						playback_set_index = workout_selected_set_index;
						playback_exercise_log = find_or_create_exercise_log_entry(curr_workout_date, playback_exercise);
						playback_set_log = find_or_create_set_log_entry(playback_exercise_log, playback_set);
						init_int(playback_set_log->started_at, now());
						set_playback_state(EXERCISE_SETTING_UP);
					}
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

void tick() {
	long int diff;

	if (app_current_screen == WORKOUT) {
		time_since_last_tick = now() - playback_last_state_change;
		diff = time_since_last_tick;
		//debug_print("[%s] diff: %ld", STATE_STR[playback_state], diff);
		if (diff >= playback_stay_seconds) {
			debug_print("MOVE STATE    before = %d [%s]", playback_state, STATE_STR[playback_state]);
			switch (playback_state) {
				case EXERCISE_SETTING_UP:
					if (playback_set_log) {
						playback_set_log->started_at = now();
					}
					if (playback_exercise_index == 0) {
						curr_workout_date->started_at = now();
					}
					if (playback_exercise_log != NULL && playback_set_index == 0) {
						playback_exercise_log->started_at = now();
					}
					//playback_last_state_change = now();
					set_playback_state(EXERCISE_INPROGRESS);
					break;
				case EXERCISE_INPROGRESS:
					if (playback_set_log) {
						playback_set_log->completed_at = now();
					}
					if (playback_is_last_set) {
						playback_exercise_log->last_completed_at = now();
					}
					//playback_last_state_change = now();
					set_playback_state(EXERCISE_RESTING);
					break;
				case EXERCISE_RESTING:
					/* fun part - move to the next set or exercise */
					if (playback_set_log == NULL) return;
					playback_set_log->done_rested_at = now();
					debug_print("Done resting this set.  Currently at playback_set_index %d and there are %d sets in this exercise", playback_set_index, playback_exercise->num_sets);
					if (playback_set_index >= playback_exercise->num_sets - 1) {
						playback_exercise_log->finished_at = now();
						/* current exercise is done */
						if (playback_exercise_index >= curr_workout->num_exercises - 1) {
							/* done workout! */
							curr_workout_date->finished_at = now();
							set_playback_state(DONE_WORKOUT);
						} else {
							/* still have more exercises in the workout */
							playback_exercise_index++;
							playback_exercise = curr_workout->exercises[playback_exercise_index];
							playback_exercise_log = find_or_create_exercise_log_entry(curr_workout_date, playback_exercise);
							playback_set_index = 0;
							playback_set = playback_exercise->sets[0]; /* TODO: what if there are no sets? */
							playback_set_log = find_or_create_set_log_entry(playback_exercise_log, playback_set);
							set_playback_state(EXERCISE_SETTING_UP);
						}
					} else {
						/* still have more sets in exercise */
						playback_set_index++;
						playback_set++;
						playback_set_log = find_or_create_set_log_entry(playback_exercise_log, playback_set);
						playback_is_last_set = playback_set_index + 1 == playback_exercise->num_sets;
						set_playback_state(EXERCISE_SETTING_UP);
					}
					//playback_last_state_change = now();
					break;
			}
			debug_print("              after = %d [%s]", playback_state, STATE_STR[playback_state]);
		}

		/* save logs if necessary */
		/*
		if (now() - last_save > 10) {
			debug_print("SAVE LOGS");
			save_all_logs();
			last_save = now();
		}
		*/
	}
}

workout *find_workout(long id) {
	int i;
	for (i = 0; i < num_workouts; i++) {
		if (workouts[i].id == id) {
			return workouts + i;
		}
	}
	return NULL;
}

exercise *find_exercise(long id) {
	int i;
	for (i = 0; i < num_exercises; i++) {
		if (exercises[i].id == id) {
			return exercises + i;
		}
	}
	return NULL;
}

workout_date *find_workout_date(long id) {
	int i;
	for (i = 0; i < num_workout_dates; i++) {
		if (workout_dates[i].id == id) {
			return workout_dates + i;
		}
	}
	return NULL;
}

void init_debug() {
	if (DEBUG_ENABLED) {
		debug_fd = rb->open("/workout.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
	}
}

void debug(char *s) {
	if (DEBUG_ENABLED) {
		if (s[rb->strlen(s)-1] != '\n') {
			rb->fdprintf(debug_fd, "%s\n", s);
		} else {
			rb->fdprintf(debug_fd, "%s", s);
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
	debug("Screen switch to workout menu");
	app_current_screen = WORKOUT_MENU;
	clear_screen();
}

void set_screen_to_workout() {
	exercise *e;
	exercise_set *s;
	exercise_log_entry *ele;
	set_log_entry *sle;

	debug("Screen switch to workout view");
	if (app_current_screen == WORKOUT_MENU) {
		curr_workout_date = workout_dates + workout_menu_selected_row;
		curr_workout = curr_workout_date->workout;
		curr_workout_date->workout = curr_workout;
		debug_print("Number of exercise in workout: %d\n", curr_workout->num_exercises);
		if (curr_workout->num_exercises > 0) {
			workout_selected_exercise = curr_workout->exercises[0];
			workout_selected_exercise_index = 0;

			// set exercise_log_entry of the first exercise
			e = curr_workout->exercises[0];
			ele = find_or_create_exercise_log_entry(curr_workout_date, e);
			playback_exercise = e;
			playback_exercise_index = 0;
			playback_exercise_log = ele;
			
			/* set the first set created */
			debug_print("Checking for first set entry.  Num sets: %d", e->num_sets);
			if (e->num_sets > 0) {
				s = e->sets[0];
				sle = find_or_create_set_log_entry(ele, s);
				playback_set = s;
				playback_set_index = 0;
				playback_set_log = sle;
			}
			set_playback_state(EXERCISE_SETTING_UP);
		}
	}
	app_current_screen = WORKOUT;
	//playback_last_state_change = now();
	last_save = now();
	workout_reset_cache = true;
	workout_top_row = 0;
	clear_screen();
}

void setup_fake_data(void) {
	/* sets */
	/*
	exercise_sets[0].exercise_id = 0;
	rb->strcpy(exercise_sets[0].name, "A1");
	exercise_sets[0].position = 0;
	exercise_sets[1].exercise_id = 0;
	rb->strcpy(exercise_sets[1].name, "A2");
	exercise_sets[1].position = 1;
	exercise_sets[2].exercise_id = 0;
	rb->strcpy(exercise_sets[2].name, "A3");
	exercise_sets[2].position = 2;
	*/

	/* exercises */
	/*
	num_exercises = 3;
	rb->strcpy(exercises[0].name, "ExA");
	exercises[0].sets[0] = exercise_sets + 0;
	exercises[0].sets[1] = exercise_sets + 1;
	exercises[0].sets[2] = exercise_sets + 2;
	exercises[0].num_sets = 3;
	rb->strcpy(exercises[1].name, "ExB");
	rb->strcpy(exercises[2].name, "ExC");
	*/
	
	/* workouts */
	/*
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
	*/
}

void workout_menu_fwd() {
	if (workout_menu_selected_row < num_workout_dates - 1) {
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
bool workout_fwd_set(void) {
	if (workout_selected_set_index < workout_selected_exercise->num_sets - 1) {
		workout_selected_set++;
		workout_selected_set_index++;
		workout_selected_set = workout_selected_exercise->sets[workout_selected_set_index];
		workout_top_row = MAX(workout_top_row, workout_selected_set->row - WORKOUT_ROWS + 1);
		return true;
	} else {
		return false;
	}
}

/* tries to move forward one exercise.  returns true if successful, false if at the last exercise */
bool workout_fwd_exercise(void) {
	if (workout_selected_exercise_index < curr_workout->num_exercises - 1) {
		workout_selected_exercise_index++;
		workout_selected_exercise = curr_workout->exercises[workout_selected_exercise_index];
		workout_top_row = MAX(workout_top_row, workout_selected_exercise->row - WORKOUT_ROWS + 1);
		return true;
	} else {
		return false;
	}
}

/* tries to move backward one set.  return true if successful, false if at the first set of the current exercise */
bool workout_back_set(void) {
	if (workout_selected_set_index > 0) {
		workout_selected_set_index--;
		workout_selected_set = workout_selected_exercise->sets[workout_selected_set_index];
		workout_top_row = MIN(workout_top_row, workout_selected_set->row);
		return true;
	} else {
		return false;
	}
}

/* tries to move backward one exercise.  returns true if successful, false if at the last exercise */
bool workout_back_exercise(void) {
	if (workout_selected_exercise_index > 0) {
		workout_selected_exercise_index--;
		workout_selected_exercise = curr_workout->exercises[workout_selected_exercise_index];
		workout_top_row = MIN(workout_top_row, workout_selected_exercise->row);
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
			workout_top_row = MAX(workout_top_row, workout_selected_set->row - WORKOUT_ROWS + 1);
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

	/* if no set is chosen but there is an exercise chosen, go backwards to the last set of the previous exercise */
	if (workout_selected_exercise != NULL && workout_selected_set == NULL) {
		if (workout_back_exercise()) {
			ns = workout_selected_exercise->num_sets;
			workout_selected_set = workout_selected_exercise->sets[ns-1];
			workout_selected_set_index = ns-1;
			workout_top_row = MIN(workout_top_row, workout_selected_set->row);
		}
		return;
	}

	/* otherwise move the set backward one, or jump to the exercise only */
	if (!workout_back_set()) {
		workout_selected_set = NULL;
		workout_top_row = MIN(workout_top_row, workout_selected_exercise->row);
	}
}

void draw_workout_menu() {
	int i, x, y, half_row;
	bool at_top, at_bottom;

	rb->lcd_set_foreground(WORKOUT_MENU_COLOR);
	x = (SCREEN_WIDTH - WORKOUT_MENU_WIDTH) / 2;
	y = (SCREEN_HEIGHT - WORKOUT_MENU_HEIGHT) / 2;
	
	//debug_print("x=%d y=%d", x, y);
	half_row = WORKOUT_MENU_ROW_HEIGHT / 2;

	/* border */
	rb->lcd_drawrect(x, y, WORKOUT_MENU_WIDTH, WORKOUT_MENU_HEADER_HEIGHT); 
	rb->lcd_drawrect(x, y, WORKOUT_MENU_WIDTH, WORKOUT_MENU_HEIGHT); 
	rb->lcd_drawrect(x, y + WORKOUT_MENU_HEIGHT - WORKOUT_MENU_HEADER_HEIGHT, 
			WORKOUT_MENU_WIDTH, WORKOUT_MENU_HEADER_HEIGHT); 

	/* more scroll up/down triangle indicators */
	if (num_workouts > WORKOUT_MENU_ROWS) {
		at_top = workout_menu_top_item_index == 0;
		at_top = false;
		at_bottom = workout_menu_top_item_index == num_workouts - WORKOUT_MENU_ROWS;
		draw_menu_more(SCREEN_WIDTH / 2, y + WORKOUT_MENU_HEADER_HEIGHT / 2, !at_top, false); 
		draw_menu_more(SCREEN_WIDTH / 2, y + WORKOUT_MENU_HEIGHT - WORKOUT_MENU_HEADER_HEIGHT / 2, !at_bottom, true); 
	}

	/* menu */
	y += WORKOUT_MENU_HEADER_HEIGHT + WORKOUT_MENU_MARGIN;
	for (i = 0; 
	     i < WORKOUT_MENU_ROWS && i + workout_menu_top_item_index < num_workout_dates; i++) {
		// clear out the row with a solid color first
		rb->lcd_set_foreground(BACKGROUND_COLOR);
		rb->lcd_fillrect(x + WORKOUT_MENU_MARGIN, y, WORKOUT_MENU_WIDTH - WORKOUT_MENU_MARGIN*2, WORKOUT_MENU_ROW_HEIGHT);
		// set different color if selected
		if (i + workout_menu_top_item_index == workout_menu_selected_row) {
			rb->lcd_set_foreground(WORKOUT_MENU_SEL_COLOR);
		} else {
			rb->lcd_set_foreground(WORKOUT_MENU_COLOR);
		}
		// row text
		rb->lcd_putsxy(x + WORKOUT_MENU_MARGIN, y, workout_dates[i + workout_menu_top_item_index].workout->name);

		// workout date
		y += half_row;
		rb->lcd_putsxy(LCD_WIDTH * 0.3, y, workout_dates[i + workout_menu_top_item_index].when);
		y += half_row;
		y += 2;
		//y += WORKOUT_MENU_ROW_HEIGHT;
	}
}

void draw_workout() {
	draw_workout_buffer();
}

void draw_workout_buffer() {
	int row, i, j, x, y, c, half_row, title_width;
	//bool at_top, at_bottom;
	char ex_line[STR_LEN], set_line[STR_LEN];
	exercise* curr_exercise;
	exercise_set *curr_set;
	exercise_log_entry *exercise_log_entry;
	set_log_entry *set_log_entry;
	bool draw;
	
	/* reset buffers, they will be rendered again from here */
	next_free_buffer = 0;

	/* start with a blank screen */
	rb->lcd_set_foreground(LCD_BLACK);
	rb->lcd_fillrect(0, 0, LCD_WIDTH, LCD_HEIGHT);

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
	//debug_print("In workout view render.  Current workout: %s; num exc: %d", curr_workout->name, curr_workout->num_exercises);
	row = 0;
	for (i = 0; i < curr_workout->num_exercises; i++, row++, y += (draw ? WORKOUT_ROW_HEIGHT : 0)) {
		draw = row >= workout_top_row && row < workout_top_row + WORKOUT_ROWS;
		curr_exercise = curr_workout->exercises[i + workout_top_item_index];

		// find exercise log
		exercise_log_entry = find_exercise_log_entry(curr_workout_date, curr_exercise);

		//debug_print("In workout view render loop.  workout_top_row: %d  row: %d  draw: %d", workout_top_row, row, draw);
		// clear out the row with a solid color first
		if (draw) {
			rb->lcd_set_foreground(BACKGROUND_COLOR);
			rb->lcd_fillrect(0, y, SCREEN_WIDTH, WORKOUT_ROW_HEIGHT);
		}

		/* exercise text */
		if (exercise_log_entry && exercise_log_entry->finished_at != 0) {
			rb->lcd_set_foreground(WORKOUT_DONE_COLOR);
		} else if (exercise_log_entry && exercise_log_entry->last_completed_at != 0) {
			rb->lcd_set_foreground(WORKOUT_RESTING_COLOR);
		} else if (exercise_log_entry && exercise_log_entry->started_at != 0) {
			rb->lcd_set_foreground(WORKOUT_INPROGRESS_COLOR);
		} else if (exercise_log_entry && exercise_log_entry->created_at != 0) {
			rb->lcd_set_foreground(WORKOUT_SETUP_COLOR);
		} else {
			rb->lcd_set_foreground(WORKOUT_COLOR);
		}
		if (playback_exercise == curr_exercise && playback_set == NULL) {
			rb->snprintf(ex_line, STR_LEN, "*%s", curr_exercise->name);
			rb->lcd_putsxy(WORKOUT_MARGIN + 1, y, ex_line);
		} else {
			rb->lcd_putsxy(WORKOUT_MARGIN + 1, y, curr_exercise->name);
		}

		/* draw box around it if selected */
		if (draw && workout_selected_set == NULL && workout_selected_exercise == curr_exercise) {
			rb->lcd_drawrect(WORKOUT_MARGIN, y, SCREEN_WIDTH - WORKOUT_MARGIN * 2, WORKOUT_ROW_HEIGHT - 1);
		}

		if (workout_reset_cache) {
			curr_exercise->row = row;
		}

		/* exercise sets */
		row++;
		if (draw) y += WORKOUT_ROW_HEIGHT;
		c = 0;
		for (j = 0; j < curr_workout->exercises[i + workout_top_item_index]->num_sets; j++) {
			draw = row >= workout_top_row && row < workout_top_row + WORKOUT_ROWS;
			curr_set = curr_workout->exercises[i + workout_top_item_index]->sets[j];
			x = c * WORKOUT_SET_WIDTH + WORKOUT_MARGIN + WORKOUT_SET_MARGIN;

			/* clear out space */
			if (draw) {
				rb->lcd_set_foreground(BACKGROUND_COLOR);
				rb->lcd_fillrect(x, y, WORKOUT_SET_WIDTH, WORKOUT_ROW_HEIGHT);
			}

			/* set text */
			set_log_entry = find_set_log_entry(exercise_log_entry, curr_set);

			if (draw) {
				if (set_log_entry && set_log_entry->done_rested_at != 0) {
					rb->lcd_set_foreground(WORKOUT_DONE_COLOR);
				} else if (set_log_entry && set_log_entry->completed_at != 0) {
					rb->lcd_set_foreground(WORKOUT_RESTING_COLOR);
				} else if (set_log_entry && set_log_entry->started_at != 0) {
					rb->lcd_set_foreground(WORKOUT_INPROGRESS_COLOR);
				} else if (set_log_entry && set_log_entry->created_at != 0) {
					rb->lcd_set_foreground(WORKOUT_SETUP_COLOR);
				} else {
					rb->lcd_set_foreground(WORKOUT_COLOR);
				}
			}
			
			if (workout_reset_cache) {
				if (curr_exercise->exercise_type_id == EXERCISE_TYPE_WEIGHTS) {
					curr_set->reps = calculate_function("reps", curr_set, curr_workout_date->n, 0);
					curr_set->weight = calculate_function("weight", curr_set, curr_workout_date->n, 0);
				} else if (curr_exercise->exercise_type_id == EXERCISE_TYPE_STRETCH) {
					curr_set->hold_for = calculate_function("hold_for", curr_set, curr_workout_date->n, 30);
				}
				curr_set->row = row;
			}

			/* draw set */
			if (draw) {
				//debug_print("    exercise_type_id=%d", curr_exercise->exercise_type_id);
				if (curr_exercise->exercise_type_id == EXERCISE_TYPE_WEIGHTS) {
					if (curr_set == playback_set) {
						rb->snprintf(set_line, STR_LEN, "*%s %dx%d", curr_set->name, (int)curr_set->reps, (int)curr_set->weight);
					} else {
						rb->snprintf(set_line, STR_LEN, "%s %dx%d", curr_set->name, (int)curr_set->reps, (int)curr_set->weight);
					}
				} else if (curr_exercise->exercise_type_id == EXERCISE_TYPE_STRETCH) {
					if (curr_set == playback_set) {
						rb->snprintf(set_line, STR_LEN, "*%s %ds", curr_set->name, (int)curr_set->hold_for);
					} else {
						rb->snprintf(set_line, STR_LEN, "%s %ds", curr_set->name, (int)curr_set->hold_for);
					}
				}
				rb->lcd_putsxy(x + 1, y + 1, set_line);
			}

			/* draw box around it if selected */
			if (draw && workout_selected_set == curr_set) {
				//debug_print("   select set %s", curr_set->name);
				rb->lcd_drawrect(x, y, WORKOUT_SET_WIDTH, WORKOUT_ROW_HEIGHT - 1);
			}

			/* increment set column */
			c++;
			if (c == WORKOUT_SETS_PER_ROW) {
				row++;
				if (draw) y += WORKOUT_ROW_HEIGHT;
				draw = row >= workout_top_row && row < workout_top_row + WORKOUT_ROWS;
				c = 0;
			}

		}


		if (row == WORKOUT_ROWS) {
			/*
			copy_screen_to_buffer();
			row = 0;
			y = 0;
			*/
		}
	}

	workout_reset_cache = false;
	draw_workout_dashboard();
	copy_screen_to_buffer();
}

void draw_workout_dashboard() {
	int top, left, w, h;
	float percent_complete;
	char line[STR_LEN];

	/* start with all back */
	rb->lcd_set_foreground(LCD_BLACK);
	rb->lcd_fillrect(0, WORKOUT_DASHBOARD_TOP, LCD_WIDTH, LCD_HEIGHT - WORKOUT_DASHBOARD_TOP);

	/* draw a simple hr divider */
	rb->lcd_set_foreground(WORKOUT_COLOR);
	rb->lcd_hline(0, LCD_WIDTH, WORKOUT_DASHBOARD_TOP);

	/* progress bar */
	top = WORKOUT_DASHBOARD_TOP + WORKOUT_DASHBOARD_PROG_TOP;
	left = WORKOUT_DASHBOARD_PROG_LEFT;
	w = LCD_WIDTH - WORKOUT_DASHBOARD_PROG_LEFT * 2;
	h = WORKOUT_DASHBOARD_PROG_HEIGHT;
	rb->lcd_set_foreground(WORKOUT_COLOR);
	rb->lcd_drawrect(left, top, w, h);

	/* calculate percent */
	percent_complete = MIN(((float)time_since_last_tick / playback_stay_seconds) * 100.0, 100.0);

	/* fill in inside based on percent */
	top++;
	left++;
	w -= 2;
	h -= 2;
	// draw percent complete
	rb->lcd_set_foreground(WORKOUT_COLOR);
	w = (percent_complete / 100.0) * w;
	rb->lcd_fillrect(left, top, w, h);

	// print the state
	rb->lcd_set_foreground(LCD_BLACK);
	rb->lcd_fillrect(LCD_WIDTH * 0.333, WORKOUT_DASHBOARD_TOP + WORKOUT_DASHBOARD_PROG_HEIGHT + 30, LCD_WIDTH * 0.333, 20);
	rb->lcd_set_foreground(WORKOUT_COLOR);
	if (playback_exercise == NULL ) {
		// noop
	} else if (playback_exercise->exercise_type_id == EXERCISE_TYPE_WEIGHTS) {
		if (playback_state == EXERCISE_SETTING_UP) {
			rb->snprintf(line, STR_LEN, "%s [%ds] %d lb", STATE_STR[playback_state], (int)playback_stay_seconds, (int)playback_set->weight);
		} else {
			rb->snprintf(line, STR_LEN, "%s [%ds]", STATE_STR[playback_state], (int)playback_stay_seconds);
		}
	} else if (playback_exercise->exercise_type_id == EXERCISE_TYPE_STRETCH) {
		rb->snprintf(line, STR_LEN, "%s [%ds]", STATE_STR_FOR_STRETCH[playback_state], (int)playback_stay_seconds);
	}
	rb->lcd_putsxy(LCD_WIDTH * 0.15, WORKOUT_DASHBOARD_TOP + WORKOUT_DASHBOARD_PROG_HEIGHT + 30, line);
}

void copy_screen_to_buffer() {
/*
	if (next_free_buffer == MAX_BUFFERS) {
		//debug("ERROR: out of buffers!");
	} else {
		rb->memcpy(buffers[next_free_buffer], 
				rb->lcd_framebuffer, LCD_WIDTH * LCD_HEIGHT * LCD_DEPTH);
		next_free_buffer++;
	}
*/
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

void workout_loaded(char cname[STR_LEN], char type[STR_LEN], char value[STR_LEN]) {
	rb->snprintf(debug_line_ptr, STR_LEN, "workout_loaded name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	debug(debug_line);
	if (rb->strcmp(cname, "id") == 0) {
		debug("NEW WORKOUT");
		num_workouts++;
		workouts[num_workouts-1].id = rb->atoi(value);
		// inits
		workouts[num_workouts-1].num_exercises = 0;
	} else if (rb->strcmp(cname, "name") == 0) {
		debug("COPY NAME");
		rb->strcpy(workouts[num_workouts-1].name, value);
	}
}

void exercise_loaded(char cname[STR_LEN], char type[STR_LEN], char value[STR_LEN]) {
	debug_print("exercise loaded. name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	if (rb->strcmp(cname, "id") == 0) {
		debug("NEW EXERCISE");
		num_exercises++;
		exercises[num_exercises-1].id = rb->atoi(value);
		// inits
		exercises[num_exercises-1].num_sets = 0;
	} else if (rb->strcmp(cname, "exercise_type_id") == 0) {
		exercises[num_exercises-1].exercise_type_id = rb->atoi(value);
	} else if (rb->strcmp(cname, "name") == 0) {
		debug("COPY NAME");
		rb->strcpy(exercises[num_exercises-1].name, value);
	}
}

void workout_exercise_loaded(char cname[STR_LEN], char type[STR_LEN], char value[STR_LEN]) {
	debug_print("workout_exercise loaded. name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	if (rb->strcmp(cname, "id") == 0) {
		debug("NEW WORKOUT EXERCISE");
		num_workout_exercises++;
		workout_exercises[num_workout_exercises-1].id = rb->atoi(value);
	} else if (rb->strcmp(cname, "workout_id") == 0) {
		workout_exercises[num_workout_exercises-1].workout_id = rb->atoi(value);
	} else if (rb->strcmp(cname, "exercise_id") == 0) {
		workout_exercises[num_workout_exercises-1].exercise_id = rb->atoi(value);
		workout *w = find_workout(workout_exercises[num_workout_exercises-1].workout_id);
		exercise *e = find_exercise(workout_exercises[num_workout_exercises-1].exercise_id);
		w->num_exercises++;
		w->exercises[w->num_exercises-1] = e;
	}
}

void exercise_set_loaded(char cname[STR_LEN], char type[STR_LEN], char value[STR_LEN]) {
	exercise *e;

	debug_print("workout_exercise_set loaded. name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	if (rb->strcmp(cname, "id") == 0) {
		debug("NEW WORKOUT EXERCISE SET");
		num_exercise_sets++;
		exercise_sets[num_exercise_sets-1].id = rb->atoi(value);
	} else if (rb->strcmp(cname, "name") == 0) {
		debug("COPY NAME");
		rb->strcpy(exercise_sets[num_exercise_sets-1].name, value);
	} else if (rb->strcmp(cname, "exercise_id") == 0) {
		e = find_exercise(rb->atoi(value));
		exercise_sets[num_exercise_sets-1].exercise_ref = e;
		e->num_sets++;
		e->sets[e->num_sets-1] = exercise_sets + num_exercise_sets - 1;
	}
}

void workout_date_loaded(char cname[STR_LEN], char type[STR_LEN], char value[STR_LEN]) {
	//rb->snprintf(debug_line_ptr, STR_LEN, "workout_date_loaded name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	debug_print("workout_date loaded. name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	debug(debug_line);
	if (rb->strcmp(cname, "id") == 0) {
		debug("NEW WORKOUT DATE");
		num_workout_dates++;
		workout_dates[num_workout_dates-1].id = rb->atoi(value);
	} else if (rb->strcmp(cname, "workout_id") == 0) {
		debug("COPY workout_id");
		//workout_dates[num_workout_dates-1].workout_id = rb->atoi(value);
		workout_dates[num_workout_dates-1].workout = find_workout(rb->atoi(value));
		debug_print("       name of workout: %s", workout_dates[num_workout_dates-1].workout->name);
	} else if (rb->strcmp(cname, "n") == 0) {
		debug("COPY n");
		workout_dates[num_workout_dates-1].n = rb->atoi(value);
	} else if (rb->strcmp(cname, "when_int") == 0) {
		debug("COPY n");
		workout_dates[num_workout_dates-1].when_int = rb->atoi(value);
	} else if (rb->strcmp(cname, "when") == 0) {
		debug("COPY when");
		rb->strlcpy(workout_dates[num_workout_dates-1].when, value, 11);
		workout_dates[num_workout_dates-1].when[12] = 0;
	}
}

void exercise_log_loaded(char cname[STR_LEN], char type[STR_LEN], char value[STR_LEN]) {
	debug_print("exercise_log_loaded name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	
	if (rb->strcmp(cname, "id") == 0) {
		debug("NEW EXERCISE LOG");
		num_exercise_log_entries++;
	} else if (rb->strcmp(cname, "created_at_int") == 0) {
		exercise_log_entries[num_exercise_log_entries-1].created_at = rb->atoi(value);
	} else if (rb->strcmp(cname, "started_at_int") == 0) {
		exercise_log_entries[num_exercise_log_entries-1].started_at = rb->atoi(value);
	} else if (rb->strcmp(cname, "last_completed_at_int") == 0) {
		exercise_log_entries[num_exercise_log_entries-1].last_completed_at = rb->atoi(value);
	} else if (rb->strcmp(cname, "finished_at_int") == 0) {
		exercise_log_entries[num_exercise_log_entries-1].finished_at = rb->atoi(value);
	} else if (rb->strcmp(cname, "workout_date_id") == 0) {
		exercise_log_entries[num_exercise_log_entries-1].workout_date = find_workout_date(rb->atoi(value));
		debug_print("%d",exercise_log_entries[num_exercise_log_entries-1].workout_date->id);
	} else if (rb->strcmp(cname, "exercise_id") == 0) {
		exercise_log_entries[num_exercise_log_entries-1].exercise = find_exercise(rb->atoi(value));
		debug_print("%d", exercise_log_entries[num_exercise_log_entries-1].exercise->id);
	} 
}

void set_log_loaded(char cname[STR_LEN], char type[STR_LEN], char value[STR_LEN]) {
	rb->snprintf(debug_line_ptr, STR_LEN, "set_log_loaded name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	debug(debug_line);
	
	if (rb->strcmp(cname, "id") == 0) {
		debug("NEW SET LOG");
		num_set_log_entries++;
	} else if (rb->strcmp(cname, "created_at_int") == 0) {
		set_log_entries[num_set_log_entries-1].created_at = rb->atoi(value);
	} else if (rb->strcmp(cname, "started_at_int") == 0) {
		set_log_entries[num_set_log_entries-1].started_at = rb->atoi(value);
	} else if (rb->strcmp(cname, "completed_at_int") == 0) {
		set_log_entries[num_set_log_entries-1].completed_at = rb->atoi(value);
	} else if (rb->strcmp(cname, "done_rested_at_int") == 0) {
		set_log_entries[num_set_log_entries-1].done_rested_at = rb->atoi(value);
	} else if (rb->strcmp(cname, "exercise_set_id") == 0) {
		set_log_entries[num_set_log_entries-1].exercise_set = find_exercise_set(rb->atoi(value));
	} else if (rb->strcmp(cname, "exercise_log_entry_created_at_int") == 0) {
		set_log_entries[num_set_log_entries-1].exercise_log_entry = find_exercise_log_entry_from_created_at(rb->atoi(value));
		debug_print("exercise_log_entry_created_at_int,integer,%ld\n", set_log_entries[num_set_log_entries-1].exercise_log_entry->created_at);
	} else if (rb->strcmp(cname, "n") == 0) {
		set_log_entries[num_set_log_entries-1].n = rb->atoi(value);
	} else if (rb->strcmp(cname, "position") == 0) {
		set_log_entries[num_set_log_entries-1].position = rb->atoi(value);
	}
}

void function_loaded(char cname[STR_LEN], char type[STR_LEN], char value[STR_LEN]) {
	int value_i;

	debug_print("function loaded name: {%s} type: {%s} value: {%s}\n", cname, type, value);
	/* TODO */
	if (rb->strcmp(cname, "id") == 0) {
		debug("NEW FUNCTION");
		num_functions++;
		functions[num_functions-1].id = rb->atoi(value);
		functions[num_functions-1].base = 0;
		functions[num_functions-1].inc = 0;
		functions[num_functions-1].round_to_nearest = 0;
		functions[num_functions-1].min_n = -INT_MAX;
		functions[num_functions-1].max_n = INT_MAX;
		functions[num_functions-1].min_v = -INT_MAX;
		functions[num_functions-1].max_v = INT_MAX;
		functions[num_functions-1].variable[0] = 0;
	} else if (rb->strcmp(cname, "base") == 0) {
		functions[num_functions-1].base = rb->atoi(value);
	} else if (rb->strcmp(cname, "inc") == 0) {
		functions[num_functions-1].inc = rb->atoi(value);
	} else if (rb->strcmp(cname, "round_to_nearest") == 0) {
		functions[num_functions-1].round_to_nearest = rb->atoi(value);
	} else if (rb->strcmp(cname, "min_n") == 0) {
		functions[num_functions-1].min_n = rb->atoi(value);
	} else if (rb->strcmp(cname, "max_n") == 0) {
		value_i = rb->atoi(value);
		if (value_i != 0) {
			functions[num_functions-1].max_n = rb->atoi(value);
		}
	} else if (rb->strcmp(cname, "min_v") == 0) {
		functions[num_functions-1].min_v = rb->atoi(value);
	} else if (rb->strcmp(cname, "max_v") == 0) {
		value_i = rb->atoi(value);
		if (value_i != 0) {
			functions[num_functions-1].max_v = rb->atoi(value);
		}
	} else if (rb->strcmp(cname, "variable") == 0) {
		rb->strcpy(functions[num_functions-1].variable, value);
	} else if (rb->strcmp(cname, "exercise_set_id") == 0) {
		functions[num_functions-1].exercise_set = find_exercise_set(rb->atoi(value));
	}
}

void load_workouts(void) {
	read_csv("/workout/workouts.csv", workout_loaded);
}

void load_exercises(void) {
	read_csv("/workout/exercises.csv", exercise_loaded);
}

void load_workout_exercises(void) {
	read_csv("/workout/workout_exercises.csv", workout_exercise_loaded);
}

void load_exercise_sets(void) {
	read_csv("/workout/exercise_sets.csv", exercise_set_loaded);
}

void load_workout_dates(void) {
	//read_csv("/workout/workout_dates.csv", workout_date_loaded);
	read_csv("/workout_dates.csv", workout_date_loaded);
}

void load_exercise_logs(void) {
	read_csv("/exercise_logs.csv", exercise_log_loaded);
}

void load_set_logs(void) {
	read_csv("/set_logs.csv", set_log_loaded);
}

void load_functions(void) {
	read_csv("/workout/functions.csv", function_loaded);
}

void load_csvs() {
	load_workouts();
	load_exercises();
	load_workout_exercises();
	load_exercise_sets();
	load_workout_dates();
	load_functions();

	// logs
	load_exercise_logs();
	load_set_logs();
}

int read_csv(char *name, void (*callback)(char[STR_LEN], char[STR_LEN], char[STR_LEN])) {
	char line[TXT_LEN], cname[STR_LEN], type[STR_LEN], value[STR_LEN];
	unsigned int i, last, section;
	int fd;

	//rb->snprintf(debug_line_ptr, STR_LEN, "read_csv START: %s", name);
	debug_print("read_csv START: %s", name);
	//debug(debug_line);
	fd = rb->open(name, O_RDONLY);

	while (rb->read_line(fd, line, TXT_LEN - 1) > 0) {
		debug(line);

		// split by commas
		section = last = 0;
		value[0] = 0;
		for (i = 0; i < rb->strlen(line); i++) {
			if (line[i] == ',' || i == rb->strlen(line) - 1) {
				if (section == 0) {
					strncpy(cname, line + last, i - last);
					cname[i - last] = 0;
				} else if (section == 1) {
					strncpy(type, line + last, i - last);
					type[i - last] = 0;
				} else if (section == 2) {
					strncpy(value, line + last, i - last + 1);
					value[i - last + 1] = 0;
				}
				section++;
				last = i + 1;
			}
		}

		//printf("name: {%s} type: {%s} value: {%s}\n", cname, type, value);
		(*callback)(cname, type, value);
	}

	rb->close(fd);
	//rb->snprintf(debug_line_ptr, STR_LEN, "read_csv END: %s", name);
	debug_print("read_csv END: %s", name);
	//debug(debug_line);

	return true;
}

exercise_log_entry *find_exercise_log_entry(workout_date *wd, exercise *e) {
	exercise_log_entry *ele;
	int i;
	for (i = 0; i < num_exercise_log_entries; i++) {
		ele = exercise_log_entries + i;
		if (ele->workout_date == wd && ele->exercise == e) {
			return ele;
		}
	}
	return NULL;
}

exercise_log_entry *find_exercise_log_entry_from_created_at(time_t created_at) {
	exercise_log_entry *ele;
	int i;
	for (i = 0; i < num_exercise_log_entries; i++) {
		ele = exercise_log_entries + i;
		if (ele->created_at == created_at) {
			return ele;
		}
	}
	return NULL;
}

exercise_log_entry *find_or_create_exercise_log_entry(workout_date *wd, exercise *e) {
	debug_print("find_or_create_exercise_log_entry workout_date=%d exercise=%d", wd->id, e->id);
	exercise_log_entry *ele;
	ele = find_exercise_log_entry(wd, e);
	if (ele == NULL) {
		debug_print("DID NOT FIND");
		num_exercise_log_entries++;
		exercise_log_entries[num_exercise_log_entries-1].created_at = now();
		exercise_log_entries[num_exercise_log_entries-1].exercise = e;
		exercise_log_entries[num_exercise_log_entries-1].workout_date = wd;
		ele = exercise_log_entries + num_exercise_log_entries - 1;
	} else {
		debug_print("found ele created_at=%ld", ele->created_at);
	}
	return ele;
}

set_log_entry *find_set_log_entry(exercise_log_entry *ele, exercise_set *s) {
	set_log_entry *sle;
	int i;
	for (i = 0; i < num_set_log_entries; i++) {
		sle = set_log_entries + i;
		if (sle->exercise_log_entry == ele && sle->exercise_set == s) {
			return sle;
		}
	}
	return NULL;
}

set_log_entry *find_or_create_set_log_entry(exercise_log_entry *ele, exercise_set *s) {
	set_log_entry *sle;
	sle = find_set_log_entry(ele, s);
	if (sle == NULL) {
		num_set_log_entries++;
		set_log_entries[num_set_log_entries-1].created_at = now();
		set_log_entries[num_set_log_entries-1].exercise_log_entry = ele;
		set_log_entries[num_set_log_entries-1].exercise_set = s;
		sle = set_log_entries + num_set_log_entries - 1;
	}
	return sle;
}

exercise_set *find_exercise_set(int id) {
	int i;
	for (i = 0; i < id; i++) {
		if (exercise_sets[i].id == id)
			return exercise_sets + i;
	}
	return NULL;
}

time_t now(void) {
	return rb->mktime(rb->get_time());
}

float calculate_function(char variable[STR_LEN], exercise_set *es, long n, float default_val) {
	long i;

	debug_print("calculate_function.  PARAMS: exercise_set=%p variable=%s n=%ld default_val=%f", es, variable, n, default_val);
	/* loop through all functions and try to find one that matches */
	for (i = 0; i < num_functions; i++) {
		debug_print("calculate_function.  FUNCTION: exercise_set=%p variable=%s min_n=%d max_n=%d", functions[i].exercise_set, functions[i].variable, functions[i].min_n, functions[i].max_n);
		if (functions[i].exercise_set == es &&
				rb->strcmp(variable, functions[i].variable) == 0 &&
				n >= functions[i].min_n &&
				n <= functions[i].max_n) {
			debug_print("calculate function.   MATCH");
			return MIN(MAX(functions[i].base + functions[i].inc * n, functions[i].min_n), functions[i].max_n) + 0.10;
		}
	}

	return default_val + 0.10;
}

void set_playback_state(int state) {
	float default_value;
	playback_state = state;
	if (playback_exercise == NULL) return;
	if (playback_exercise->exercise_type_id == EXERCISE_TYPE_WEIGHTS) {
		if (playback_set_index == 0 && playback_state == EXERCISE_SETTING_UP) {
			default_value = DEFAULT_SECONDS_ON_SETUP_FIRST_EXERCISE;
		} else {
			default_value = (float)DEFAULT_SECONDS_ON_STATE[playback_state];
		}
		playback_stay_seconds = calculate_function(STATE_WAIT_VAR[playback_state], playback_set, curr_workout_date->n, default_value);
	} else if (playback_exercise->exercise_type_id == EXERCISE_TYPE_STRETCH) {
		default_value = (float)DEFAULT_SECONDS_ON_STATE_FOR_STRETCH[playback_state];
		playback_stay_seconds = calculate_function(STATE_WAIT_VAR_FOR_STRETCH[playback_state], playback_set, curr_workout_date->n, default_value);
	}
	playback_last_state_change = now();

	// beep to notify of the new state
	rb->pcmbuf_beep(STATE_BEEP_FREQ[playback_state], STATE_BEEP_DURATION[playback_state], STATE_BEEP_AMPLITUDE[playback_state]);

	save_all_logs();
}

void write_workout_dates() {
	int i, wd_fd;
	workout_date *wd;

	wd_fd = rb->open("/workout_dates.csv", O_CREAT|O_WRONLY|O_TRUNC, 0666);
	for (i = 0; i < num_workout_dates; i++) {
		wd = workout_dates + i;
		rb->fdprintf(wd_fd, "id,integer,%d\n", wd->id);
		rb->fdprintf(wd_fd, "when,datetime,%s\n", wd->when);
		rb->fdprintf(wd_fd, "when_int,datetime,%ld\n", wd->when_int);
		rb->fdprintf(wd_fd, "workout_id,integer,%d\n", wd->workout->id);
		rb->fdprintf(wd_fd, "n,integer,%d\n", wd->n);
		rb->fdprintf(wd_fd, "started_at_int,datetime,%ld\n", wd->started_at);
		rb->fdprintf(wd_fd, "finished_at_int,datetime,%ld\n", wd->finished_at);
	}
	rb->close(wd_fd);
}

void write_exercise_logs() {
	int i, el_fd;
	exercise_log_entry *ele;

	debug_print("WRITE FILE");
	el_fd = rb->open("/exercise_logs.csv", O_CREAT|O_WRONLY|O_TRUNC, 0666);
	for (i = 0; i < num_exercise_log_entries; i++) {
		ele = exercise_log_entries + i;
		rb->fdprintf(el_fd, "id,integer\n");
		rb->fdprintf(el_fd, "created_at_int,datetime,%ld\n", ele->created_at);
		rb->fdprintf(el_fd, "started_at_int,datetime,%ld\n", ele->started_at);
		rb->fdprintf(el_fd, "last_completed_at_int,datetime,%ld\n", ele->last_completed_at);
		rb->fdprintf(el_fd, "finished_at_int,datetime,%ld\n", ele->finished_at);
		rb->fdprintf(el_fd, "workout_date_id,integer,%d\n", ele->workout_date->id);
		rb->fdprintf(el_fd, "exercise_id,integer,%d\n", ele->exercise->id);
	}
	rb->close(el_fd);
}

void write_set_log_entries() {
	int i, sle_fd;
	set_log_entry *sle;

	sle_fd = rb->open("/set_logs.csv", O_CREAT|O_WRONLY|O_TRUNC, 0666);
	for (i = 0; i < num_set_log_entries; i++) {
		sle = set_log_entries + i;
		rb->fdprintf(sle_fd, "id,integer\n");
		rb->fdprintf(sle_fd, "created_at_int,datetime,%ld\n", sle->created_at);
		rb->fdprintf(sle_fd, "started_at_int,datetime,%ld\n", sle->started_at);
		rb->fdprintf(sle_fd, "completed_at_int,datetime,%ld\n", sle->completed_at);
		rb->fdprintf(sle_fd, "done_rested_at_int,datetime,%ld\n", sle->done_rested_at);
		rb->fdprintf(sle_fd, "exercise_set_id,integer,%d\n", sle->exercise_set->id);
		rb->fdprintf(sle_fd, "exercise_log_entry_created_at_int,datetime,%ld\n", sle->exercise_log_entry->created_at);
		rb->fdprintf(sle_fd, "n,integer,%d\n", sle->n);
		rb->fdprintf(sle_fd, "position,integer,%d\n", sle->position);
	}
	rb->close(sle_fd);
}

void save_all_logs(void) {
	write_workout_dates();
	write_exercise_logs();
	write_set_log_entries();
}

void morse_play_int(int val) {
	char int_s[STR_LEN];
	unsigned short int i;

	rb->snprintf(int_s, STR_LEN, "%d", val);
	for (i = 0; i < rb->strlen(int_s); i++) {
		morse_play(int_s[i]);
	}
}

void morse_play(char c) {
	if (c == '0') {
		morse_dash();
		morse_dash();
		morse_dash();
		morse_dash();
		morse_dash();
	} else if (c == '1') {
		morse_dot();
		morse_dash();
		morse_dash();
		morse_dash();
		morse_dash();
	} else if (c == '2') {
		morse_dot();
		morse_dot();
		morse_dash();
		morse_dash();
		morse_dash();
	} else if (c == '3') {
		morse_dot();
		morse_dot();
		morse_dot();
		morse_dash();
		morse_dash();
	} else if (c == '4') {
		morse_dot();
		morse_dot();
		morse_dot();
		morse_dot();
		morse_dash();
	} else if (c == '5') {
		morse_dot();
		morse_dot();
		morse_dot();
		morse_dot();
		morse_dot();
	} else if (c == '6') {
		morse_dash();
		morse_dot();
		morse_dot();
		morse_dot();
		morse_dot();
	} else if (c == '7') {
		morse_dash();
		morse_dash();
		morse_dot();
		morse_dot();
		morse_dot();
	} else if (c == '8') {
		morse_dash();
		morse_dash();
		morse_dash();
		morse_dot();
		morse_dot();
	} else if (c == '9') {
		morse_dash();
		morse_dash();
		morse_dash();
		morse_dash();
		morse_dot();
	}

}

void morse_dot() {
	rb->pcmbuf_beep(2000, 100, 6000);
	rb->sleep(500);
}

void morse_dash() {
	rb->pcmbuf_beep(2000, 400, 6000);
	rb->sleep(500);
}

