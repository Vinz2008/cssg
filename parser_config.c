#include "parser_config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct parameter* parse_config_line(char* line){

}

struct config_file* parse_config_file(char* path){
    struct config_file* config_file_ptr = malloc(sizeof(struct config_file));
    char* line = malloc(sizeof(char)*100);
    FILE* f = fopen(path, "r");
    while (fgets(line, 150, f) != NULL){
        parse_config_line(line);
    }
    fclose(f);
    return config_file_ptr;
}