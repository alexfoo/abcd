#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

// WIDTHxHEIGHT
#define FIELD_DIM 4

typedef struct _field {
	char data[FIELD_DIM][FIELD_DIM];
	int offset_x, offset_y;
} field_t;

// requires int y, x; obviously
#define LOOP_FIELD for (y = 0; y < FIELD_DIM; y++) \
	for(x = 0; x < FIELD_DIM; x++)

typedef enum {
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
} direction_t;

void add_new_char(field_t *f) {
	int y, x;
	while(1) {
		y = rand() % FIELD_DIM;
		x = rand() % FIELD_DIM;

		if (f->data[y][x] == '.') {
			f->data[y][x] = 'a';
			break;
		}
	}
}

void clear_line(char *line) {
	int i;
	for (i = 0; i < FIELD_DIM; i++) {
		line[i] = '.';
	}
}

void init_playing_field(field_t *f) {
	int y;

	for (y = 0; y < FIELD_DIM; y++) {
		clear_line(f->data[y]);
	}

	f->offset_y = 5;
	f->offset_x = 5;

	// place two a:s to start playing with
	add_new_char(f);
	add_new_char(f);
}

void redraw(field_t *f) {
	int y, x;

	LOOP_FIELD {
		mvprintw(f->offset_y+y, f->offset_x+(x*2), "%c", f->data[y][x]);
	}

	// Move cursor out of the way
	move(0,0);
}

void do_move(field_t *f, direction_t dir) {
	int y, x, i, j, moved = 0;
	char save[FIELD_DIM];
	char old[FIELD_DIM];

	clear_line(save);

	switch (dir) {
	case DIR_UP:
		// Down was the first i wrote, so lets just try to reverse that
		for (x = FIELD_DIM-1; x >= 0; x--) {
			i = 0;
			clear_line(save);
			for (y = 0; y < FIELD_DIM; y++) {
				old[y] = f->data[y][x];
				if (f->data[y][x] != '.') {
					save[i++] = f->data[y][x];
					f->data[y][x] = '.';
				}
			}

			for (j = 0; j < FIELD_DIM; j++) {
				//mvprintw(20-j, 20+(x*4), "%c %d", save[j], j); dat debug
				if (save[j] != '.' && save[j] == save[j+1]) {
					save[j] += 1;
					save[j+1] = '.';
				}
			}

			// compact the saved values
			for (j = 0; j < FIELD_DIM-1; j++) {
				if (save[j+1] != '.' && save[j] == '.') {
					save[j] = save[j+1];
					save[j+1] = '.';
					// and start over
					j = 0;
				}
			}

			for (y = 0; y < FIELD_DIM; y++) {
				if (old[y] != save[y]) {
					moved = 1;
				}
				f->data[y][x] = save[y];
			}
		}
		break;
	case DIR_DOWN:
		// I spent hours on this, but it's probably not the best.
		for (x = FIELD_DIM-1; x >= 0; x--) {
			i = 0;
			clear_line(save);
			for (y = FIELD_DIM-1; y >= 0; y--) {
				old[y] = f->data[y][x];
				if (f->data[y][x] != '.') {
					save[i++] = f->data[y][x];
					f->data[y][x] = '.';
				}
			}

			for (j = 0; j < FIELD_DIM; j++) {
				//mvprintw(20-j, 20+(x*4), "%c %d", save[j], j); dat debug
				if (save[j] != '.' && save[j] == save[j+1]) {
					save[j] += 1;
					save[j+1] = '.';
				}
			}

			// compact the saved values
			for (j = 0; j < FIELD_DIM-1; j++) {
				if (save[j+1] != '.' && save[j] == '.') {
					save[j] = save[j+1];
					save[j+1] = '.';
					// and start over
					j = 0;
				}
			}

			for (y = FIELD_DIM-1; y >= 0; y--) {
				if (old[y] != save[abs(y-(FIELD_DIM-1))]) {
					moved = 1;
				}
				f->data[y][x] = save[abs(y-(FIELD_DIM-1))];
			}
		}

		break;
	case DIR_LEFT:
		for (y = 0; y < FIELD_DIM; y++) {
			i = 0;
			clear_line(save);
			for (x = 0; x < FIELD_DIM; x++) {
				old[x] = f->data[y][x];
				if (f->data[y][x] != '.') {
					save[i++] = f->data[y][x];
					f->data[y][x] = '.';
				}
			}

			for (j = 0; j < FIELD_DIM; j++) {
				//mvprintw(20-j, 20+(x*4), "%c %d", save[j], j); dat debug
				if (save[j] != '.' && save[j] == save[j+1]) {
					save[j] += 1;
					save[j+1] = '.';
				}
			}

			// compact the saved values
			for (j = 0; j < FIELD_DIM-1; j++) {
				if (save[j+1] != '.' && save[j] == '.') {
					save[j] = save[j+1];
					save[j+1] = '.';
					// and start over
					j = 0;
				}
			}

			for (x = 0; x < FIELD_DIM; x++) {
				if (old[x] != save[x]) {
					moved = 1;
				}
				f->data[y][x] = save[x];
			}
		}
		break;
	case DIR_RIGHT:
		for (y = FIELD_DIM-1; y >= 0; y--) {
			i = 0;
			clear_line(save);
			for (x = FIELD_DIM-1; x >= 0; x--) {
				old[x] = f->data[y][x];
				if (f->data[y][x] != '.') {
					save[i++] = f->data[y][x];
					f->data[y][x] = '.';
				}
			}

			for (j = 0; j < FIELD_DIM; j++) {
				//mvprintw(20-j, 20+(x*4), "%c %d", save[j], j); dat debug
				if (save[j] != '.' && save[j] == save[j+1]) {
					save[j] += 1;
					save[j+1] = '.';
				}
			}

			// compact the saved values
			for (j = 0; j < FIELD_DIM-1; j++) {
				if (save[j+1] != '.' && save[j] == '.') {
					save[j] = save[j+1];
					save[j+1] = '.';
					// and start over
					j = 0;
				}
			}

			for (x = FIELD_DIM-1; x >= 0; x--) {
				if (old[x] != save[abs(x-(FIELD_DIM-1))]) {
					moved = 1;
				}
				f->data[y][x] = save[abs(x-(FIELD_DIM-1))];
			}
		}
		break;
	}

	if (moved)
		add_new_char(f);
	redraw(f);
}

int main() {
	int c;
	int running = 1;
	field_t playing_field;

	initscr();
	noecho();

	srand(time(NULL));

	init_playing_field(&playing_field);
	redraw(&playing_field);

	while(running) {
		c = getch();

		switch(c) {
		case 'j':
			mvprintw(1, 1, "%d down     ", c);
			do_move(&playing_field, DIR_DOWN);
			break;
		case 'k':
			mvprintw(1, 1, "%d up       ", c);
			do_move(&playing_field, DIR_UP);
			break;
		case 'h':
			mvprintw(1, 1, "%d left     ", c);
			do_move(&playing_field, DIR_LEFT);
			break;
		case 'l':
			mvprintw(1, 1, "%d right    ", c);
			do_move(&playing_field, DIR_RIGHT);
			break;
		case 'q':
			running = 0;
			break;
		default:
			mvprintw(1, 1, "%d", c);
			break;
		}
	}

	endwin();

	return 0;
}
