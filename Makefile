CC=gcc
FILES=abcd.o
CFLAGS=-ggdb3 -lncurses

.c.o:
	$(CC) $(CFLAGS) $< -c

abcd: $(FILES)
	$(CC) $(CFLAGS) -o abcd $(FILES)

all: abcd

clean:
	rm -f $(FILES) abcd abcd.save
