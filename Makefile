CC = gcc
CFLAGS=-Wall -Wno-sizeof-pointer-memaccess -O2 -c -g $(shell pkg-config --cflags libmarkdown)
LDFLAGS=$(shell pkg-config --libs libmarkdown)

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif


SRCS := $(wildcard ./*.c) $(wildcard libs/*.c) $(wildcard webserver/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

all: clean cssg

cssg: $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o:%.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf cssg ./*.o ./libs/*.o temp/* out/* ./webserver/*.o

run:
	./cssg build

install: cssg
	install -d $(DESTDIR)$(PREFIX)/bin/
	install cssg $(DESTDIR)$(PREFIX)/bin
