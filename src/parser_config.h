#include <stdbool.h>
#include <stddef.h>

#ifndef PARSER_CONFIG_HEADER_
#define PARSER_CONFIG_HEADER_

#include "syntax-highlighting.h"

struct parameter {
    char* name;
    char* value_str;
};

struct config_file {
    char* path;
    struct parameter* parameters;
    size_t used;
    size_t size;
};

typedef struct config {
    char* templates_directory;
    char* img_directory;
    char* articles_directory;
    char* temp_directory;
    char* out_directory;
    struct config_file* alias_file;
    struct syntax_highlighting_config syntax_config;
} config_t;

int find_parameter_pos(const char* name, struct config_file* config);
void init_config_list(struct config_file* config, size_t initialSize);
void append_parameter_config_list(struct parameter param, struct config_file* config);
void empty_config_list(struct config_file* config);
struct parameter parse_config_line(char* line);
struct config_file* parse_config_file(char* path);
int find_parameter_pos_no_error(const char* name, struct config_file* config);

#endif