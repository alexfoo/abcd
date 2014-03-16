#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ncurses.h>

// WIDTHxHEIGHT
#define FIELD_DIM 4
#define SAVE_FILENAME "abcd.save"

typedef struct _field {
	char data[FIELD_DIM][FIELD_DIM];
	int offset_x, offset_y;
	char flags;
	int score;
} field_t;

#define FIELD_FLAG_FINISHED 0x01

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
	int y, x, free;

	LOOP_FIELD {
		if (f->data[y][x] == '.') {
			free++;
		}
	}

	while(free) {
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

void check_moves(field_t *f) {
	int y, x, free = 0;
	char c;

	// If we have free tiles, moves are possible
	LOOP_FIELD {
		if (f->data[y][x] == '.') {
			return;
		}
	}

	// Check for possible moves when the field is full, by checking neigbouring tiles
	LOOP_FIELD {
		c = f->data[y][x];
		if (	(y < FIELD_DIM-1 && f->data[y+1][x] == c) ||
			(x < FIELD_DIM-1 && f->data[y][x+1] == c) ||
			(y > 0 && f->data[y-1][x] == c) ||
			(x > 0 && f->data[y][x-1] == c)) {
			return;
		}
	}

	f->flags |= FIELD_FLAG_FINISHED;
}

void init_playing_field(field_t *f) {
	int y;

	for (y = 0; y < FIELD_DIM; y++) {
		clear_line(f->data[y]);
	}

	f->offset_y = 5;
	f->offset_x = 5;

	f->flags = 0;

	// place two a:s to start playing with
	add_new_char(f);
	add_new_char(f);
}

void redraw(field_t *f) {
	int y, x;

	mvprintw(f->offset_y-2, f->offset_x, "Score: %d", f->score);
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
				if (save[j] != '.' && save[j] == save[j+1]) {
					f->score += (save[j]+1-'a')*4;
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
				if (save[j] != '.' && save[j] == save[j+1]) {
					f->score += (save[j]+1-'a')*4;
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
				if (save[j] != '.' && save[j] == save[j+1]) {
					f->score += (save[j]+1-'a')*4;
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
				if (save[j] != '.' && save[j] == save[j+1]) {
					f->score += (save[j]+1-'a')*4;
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

	check_moves(f);
	redraw(f);
}

void save_state(field_t *f) {
	FILE *fp;
	int dimension = FIELD_DIM;
	int x, y;

	if (!(fp = fopen(SAVE_FILENAME, "w")))
		return;

	// write dimensions to the file so we don't open one with less/more letters
	fwrite(&dimension, sizeof(int), 1, fp);

	// for now we don't need to save the flags

	fwrite(&(f->score), sizeof(int), 1, fp);

	// This can probably be written more efficiently using the third argument of fwrite(), but... meh
	LOOP_FIELD {
		fwrite(&(f->data[y][x]), sizeof(char), 1, fp);
	}
	
	fclose(fp);
}

void load_state(field_t* f) {
	FILE *fp;
	int buf, y, x;
	char buf2;

	if (!(fp = fopen(SAVE_FILENAME, "r")))
		return;

	fread(&buf, sizeof(int), 1, fp);
	if (buf != FIELD_DIM) {
		printf("Savefile not compatible, remove it and try again\n");
		exit(1);
	}

	fread(&buf, sizeof(int), 1, fp);
	f->score = buf;

	LOOP_FIELD {
		fread(&buf2, sizeof(char), 1, fp);
		f->data[y][x] = buf2;
	}

	fclose(fp);

	unlink(SAVE_FILENAME);
}

int main() {
	int c;
	field_t playing_field;

	init_playing_field(&playing_field);
	load_state(&playing_field);

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand(time(NULL));

	redraw(&playing_field);

	while(!(playing_field.flags & FIELD_FLAG_FINISHED)) {
		c = getch();

		switch(c) {
		case 'j':
		case KEY_DOWN:
			do_move(&playing_field, DIR_DOWN);
			break;
		case 'k':
		case KEY_UP:
			do_move(&playing_field, DIR_UP);
			break;
		case 'h':
		case KEY_LEFT:
			do_move(&playing_field, DIR_LEFT);
			break;
		case 'l':
		case KEY_RIGHT:
			do_move(&playing_field, DIR_RIGHT);
			break;
		case 'q':
			save_state(&playing_field);
			playing_field.flags |= FIELD_FLAG_FINISHED;
			break;
		default:
			break;
		}
	}

	endwin();

	printf("Final score: %d\n", playing_field.score);

	return 0;
}
