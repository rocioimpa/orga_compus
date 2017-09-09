CFLAGS := -g -std=c99 -Wall -O0
OBJS := $(patsubst %.c,%.o,$(wildcard *.c))

tp0: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f tp0 *.o core *.asm

.PHONY: clean
