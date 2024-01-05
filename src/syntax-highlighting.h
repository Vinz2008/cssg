#include "parser_config.h"
#include <mkdio.h>

#ifndef SYNTAX_HIGHLIGHTING_CONFIG_HEADER_
#define SYNTAX_HIGHLIGHTING_CONFIG_HEADER_

struct config;

struct syntax_highlighting_config {
    char* theme;
};

void init_syntax_highlighting(MMIOT* mmiot, struct config* config);

#endif