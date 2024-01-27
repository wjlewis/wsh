CC = gcc
CFLAGS = -Wall -Wextra -pedantic -iquote include

DEPS = include/lexer.h \
	   include/parser.h \
	   include/alloc.h \
	   include/run.h

OBJS = src/lexer.o \
	   src/parser.o \
	   src/alloc.o \
	   src/run.o \
	   src/wsh.o

wsh: $(OBJS)
	$(CC) $(CFLAGS) -o wsh $(OBJS)

src/%.o: src/%.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -f $(OBJS) wsh
