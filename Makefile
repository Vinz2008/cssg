CC = gcc
CFLAGS=-Wall -pedantic -c -g $(shell pkg-config --cflags libmarkdown libmagic)
CFLAGS += -fsanitize=address -fsanitize=undefined


LDFLAGS=$(shell pkg-config --libs libmarkdown libmagic)
LDFLAGS += -fsanitize=address -fsanitize=undefined

RELEASE ?= FALSE

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

ifeq ($(RELEASE),TRUE)
	LDFLAGS += -s
	CFLAGS += -O3
else
	CFLAGS += -O2
endif

SRCS := $(wildcard src/*.c) $(wildcard libs/*.c) $(wildcard webserver/*.c)
OBJS = $(patsubst %.c,%.o,$(SRCS))

all: cssg

cssg: $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o:%.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf cssg ./src/*.o ./libs/*.o example/temp/* example/out/* ./webserver/*.o

run:
	cd example && ../cssg build

serve:
	cd example && ../cssg serve

install: cssg
	install -d $(DESTDIR)$(PREFIX)/bin/
	install cssg $(DESTDIR)$(PREFIX)/bin
