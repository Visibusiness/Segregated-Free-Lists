CC=gcc
CFLAGS=-Wall -Wextra -std=c99
EXEC=sfl

SRCS=$(wildcard *.c)

build: $(EXEC)

$(EXEC): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^ 

run_sfl: $(EXEC)
	./$(EXEC)


clean:
	rm -f $(EXEC)
