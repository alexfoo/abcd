CC=gcc
FILES=abcd.o
CFLAGS=-ggdb3 -lncurses
OTHER_FILES=abcd abcd.save

.c.o:
	$(CC) $< -c

abcd: $(FILES)
	$(CC) $(CFLAGS) -o abcd $(FILES)

all: abcd

clean:
	rm -f $(FILES) $(OTHER_FILES)
