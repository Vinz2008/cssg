# CSSG

## What it is

CSSG means C static site generator. It is a static site generator written in C.

## Advantages

- Easy to use
- Less than 50kb stripped, less than 100kb not stripped
- Easy to configure
- Instantaneous build time (less than a tenth of a second in a normal project)
- Generate static html
- Very customizable

## What it does

For now, it uses the [discount](http://www.pell.portland.or.us/~orc/Code/discount/) library to generate html from markdown.

## TODO

- [x] move pictures from img folder to out folder
- [x] add custom html pages and articles in article folder (find html files and insert templates)
- [ ] Add header with %title (for now it doesn't work)
- [ ] Replace use of config_file by config_t in other files than main.c
