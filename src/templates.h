#include "parser_config.h"


struct line {
    char line[150];
};

struct line_array {
    struct line* lines;
    size_t used;
    size_t size;
};

struct for_loop {
    struct line_array Line_array;
};

struct file_array {
    struct file* files;
    size_t used;
    size_t size;
};

struct file {
    char* path;
    char* name;
};

void insert_template(const char* html_file, config_t* config);