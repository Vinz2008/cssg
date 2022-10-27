CC=gcc
CFLAGS=-Wall -Wno-sizeof-pointer-memaccess -O2 -c -g `pkg-config --cflags libmarkdown`
LDFLAGS=`pkg-config --libs libmarkdown`



all: cssg

cssg: clean build build/cssg.o build/startswith.o build/removeCharFromString.o build/parser.o build/parser_config.o build/markdown_converter.o build/utils_file.o build/img.o
	$(CC) $(LDFLAGS) -o cssg build/cssg.o build/startswith.o build/removeCharFromString.o build/parser.o build/parser_config.o build/markdown_converter.o build/utils_file.o build/img.o

build:
	mkdir build

build/cssg.o:
	$(CC) $(CFLAGS) -o build/cssg.o main.c


build/startswith.o:
	$(CC) $(CFLAGS) -o build/startswith.o libs/startswith.c

build/utils_file.o:
	$(CC) $(CFLAGS) -o build/utils_file.o libs/utils_file.c

build/removeCharFromString.o:
	$(CC) $(CFLAGS) -o build/removeCharFromString.o libs/removeCharFromString.c

build/parser.o:
	$(CC) $(CFLAGS) -o build/parser.o parser.c

build/parser_config.o:
	$(CC) $(CFLAGS) -o build/parser_config.o parser_config.c

build/markdown_converter.o:
	$(CC) $(CFLAGS) -o build/markdown_converter.o markdown_converter.c

build/img.o:
	$(CC) $(CFLAGS) -o build/img.o img.c

clean:
	rm -rf cssg build temp/* out/*

run:
	./cssg build
