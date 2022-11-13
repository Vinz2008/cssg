#include "parser_config.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "libs/removeCharFromString.h"

int find_parameter_pos(const char* name, struct config_file* config){
    int pos = -1;
    for (int i = 0; i < config->used; i++){
        if (strcmp(name, config->parameters[i].name) == 0){
            pos = i;
            break;
        }
    }
    if (pos == -1){
        printf("ERROR : parameter %s could not be found\n", name);
        exit(1);
    }

    return pos;
}

void init_config_list(struct config_file* config, size_t initialSize){
    config->parameters = malloc(initialSize * sizeof(struct parameter));
    config->used = 0;
    config->size = initialSize;
}

void append_parameter_config_list(struct parameter param, struct config_file* config){
    if (config->used == config->size){
        config->size *= 2;
        config->parameters = realloc(config->parameters, config->size * sizeof(struct parameter));
    }
    config->parameters[config->used++] = param;
}

void empty_config_list(struct config_file* config){
    free(config->parameters);
    config->parameters = NULL;
    config->used = config->size = 0;
}

struct parameter* parse_config_line(char* line){
    removeCharFromString('\n', line);
    struct parameter* parameter_ptr = malloc(sizeof(struct parameter));
    bool equalFound = false;
    char* name = malloc(40 * sizeof(char));
    char* value = malloc(40 * sizeof(char));;
    //char* value = malloc(20 * sizeof(char));
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
    parameter_ptr->name = name;
    parameter_ptr->value_str = value;
    return parameter_ptr;
}

struct config_file* parse_config_file(char* path){
    struct config_file* config_file_ptr = malloc(sizeof(struct config_file));
    init_config_list(config_file_ptr, 1);
    config_file_ptr->path = path;
    char* line = malloc(sizeof(char)*100);
    FILE* f = fopen(path, "r");
    while (fgets(line, 100, f) != NULL){
        struct parameter* temp_param = parse_config_line(line);
        append_parameter_config_list(*temp_param, config_file_ptr);
        free(temp_param);
    }
    fclose(f);
    free(line);
    return config_file_ptr;
}