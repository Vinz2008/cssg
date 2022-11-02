#include "parser_config.h"

struct file_array {
    struct file* files;
    size_t used;
    size_t size;
};

struct file {
    char* path;
    char* name;
};

void insert_template(const char* html_file, struct config_file* root_parameter_file);