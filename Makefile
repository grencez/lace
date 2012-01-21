
CC = gcc
CFLAGS = -ansi -pedantic -Wall -Wextra
CFLAGS += -D_SVID_SOURCE
CFLAGS += -g

utils = best-match xpipe void cat1 ssh-all ujoin
util_exes = $(addprefix bin/,$(utils))

all: lace $(util_exes)

lace: lace.c futil.o
	$(CC) $(CFLAGS) \
		"-DUtilBin=\"$(PWD)/bin\"" \
		-o $@ $^

%.o: %.c
	$(CC) -c $(CFLAGS) $^ -o $@

bin/best-match: best-match.c futil.o
	$(CC) $(CFLAGS) -o $@ $^

bin/xpipe: xpipe.c
	$(CC) $(CFLAGS) -o $@ $^

bin/void: void.c
	$(CC) $(CFLAGS) -o $@ $^

bin/cat1: cat1.c
	$(CC) $(CFLAGS) -o $@ $^

bin/ssh-all: ssh-all.c
	$(CC) $(CFLAGS) -o $@ $^

bin/ujoin: ujoin.c futil.o
	$(CC) $(CFLAGS) -o $@ $^

$(util_exes): | bin

bin:
	mkdir -p bin

.PHONY: clean
clean:
	rm -f lace $(util_exes)

