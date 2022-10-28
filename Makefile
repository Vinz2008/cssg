CC=gcc
CFLAGS=-Wall -Wno-sizeof-pointer-memaccess -O2 -c -g `pkg-config --cflags libmarkdown`
LDFLAGS=`pkg-config --libs libmarkdown`

OBJS=\
libs/startswith.o \
main.o \
libs/utils_file.o \
libs/removeCharFromString.o \
parser.o \
parser_config.o \
markdown_converter.o \
templates.o \
img.o \

all: clean cssg

cssg: $(OBJS)
	$(CC) $(LDFLAGS) -o cssg $^

%.o:%.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -rf cssg ./*.o ./libs/*.o temp/* out/*

run:
	./cssg build
