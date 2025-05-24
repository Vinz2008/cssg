#include "parser_config.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include "string.h"

int find_parameter_pos_no_error(const char* name, struct config_file* config){
    int pos = -1;
    for (int i = 0; i < config->used; i++){
        if (strcmp(name, config->parameters[i].name) == 0){
            pos = i;
            break;
        }
    }
    return pos;
}

int find_parameter_pos(const char* name, struct config_file* config){
    int pos = find_parameter_pos_no_error(name, config);
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
    for (int i = 0; i < config->size; i++){
        free(config->parameters[i].name);
        free(config->parameters[i].value_str);
    }
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
    memset(name, 0, 40);
    memset(value, 0, 40);
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
    if (!f){
        printf("ERROR : couldn't open the config file %s\n", path);
        exit(1);
    }
    while (fgets(line, 100, f) != NULL){
        struct parameter* temp_param = parse_config_line(line);
        append_parameter_config_list(*temp_param, config_file_ptr);
        free(temp_param);
    }
    fclose(f);
    free(line);
    return config_file_ptr;
}