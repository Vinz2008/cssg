#include <stdbool.h>

#ifndef PARSER_CONFIG_HEADER_
#define PARSER_CONFIG_HEADER_

typedef enum {
    STRING,
    BOOL,
} type_t;

struct parameter {
    char* name;
    type_t type;
    char* value_str;
    bool value_bool;
};

struct config_file {
    char* path;
    struct parameter* parameters;
};

struct config_file* parse_config_file(char* path);

#endif