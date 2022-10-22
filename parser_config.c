#include "parser_config.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

struct parameter* parse_config_line(char* line){
    struct parameter* parameter_ptr = malloc(sizeof(struct parameter));
    bool equalFound = false;
    char name[20];
    char value[20];
    memset(name, 0, sizeof(name));
    memset(value, 0, sizeof(value));
    for (int i = 0; i < strlen(line); i++){
        if (line[i] == '='){
            equalFound = true;
        } else {
            if (equalFound){
               strncat(value, line + i, 1);
            } else {
                strncat(name, line + i, 1);
            }
        }
    }
    printf("name : %s\nvalue : %s\n", name, value);
    parameter_ptr->name = name;
    parameter_ptr->type = STRING;
    parameter_ptr->value_str = value;
    return parameter_ptr;
}

struct config_file* parse_config_file(char* path){
    struct config_file* config_file_ptr = malloc(sizeof(struct config_file));
    config_file_ptr->path = path;
    char* line = malloc(sizeof(char)*100);
    FILE* f = fopen(path, "r");
    while (fgets(line, 100, f) != NULL){
        parse_config_line(line);
    }
    fclose(f);
    return config_file_ptr;
}