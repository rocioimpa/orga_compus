CFLAGS := -g -Wall
OBJS := tp1.c palindrome.S

tp1: $(OBJS) 
	$(CC) $(CFLAGS) $(OBJS) $^ -o $@

tp1.S: 

clean:
	rm -f tp1 *.o core *.asm

.PHONY: clean
