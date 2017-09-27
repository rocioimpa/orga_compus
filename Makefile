CFLAGS := -g -std=c99 -Wall -O0
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

tp1: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f tp1 *.o core *.asm

.PHONY: clean
