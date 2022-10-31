#include "templates.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "parser.h"
#include "parser_config.h"
#include "libs/startswith.h"

bool for_mode = false;

void extract_variables(FILE* f, char* line){
    int pos = 0;
    for (int i = 0; i < strlen(line);i++){
        if (line[i] == '{' && line[i+1] == '{'){
            pos = i+2;
            printf("TEST\n");
            char* out = malloc(sizeof(char) * 40);
            int pos2 = pos;
            for (int j = 0; j < strlen(line); j++){
            if (line[pos2] == '}' && line[pos2+1] == '}'){
                break;
            }
            out[j] = line[pos2];
            pos2++;
            }
             printf("OUT : %s\n", out);
            if (strcmp("filename", out) == 0){
            printf("FILENAME\n");
            fprintf(f, "FILENAME");
            pos+=strlen(out)+3;
            i+=strlen(out)+3;
            }
            //break;
        } else {
        fputc(line[i], f);
        }
        }
        /*for (int i = pos; i < strlen(line); i++){
            fputc(line[i], f);
        }*/

}

void insert_template(const char* html_file, struct config_file* root_parameter_file){
    char* templates_directory = root_parameter_file->parameters[find_parameter_pos("templates_directory", root_parameter_file)].value_str;
    char* temp_directory = root_parameter_file->parameters[find_parameter_pos("temp_directory", root_parameter_file)].value_str;
    char* out_directory = root_parameter_file->parameters[find_parameter_pos("out_directory", root_parameter_file)].value_str;
    char line[150];
    //FILE* f = fopen(argv[1], "r");
    char* main_filename = malloc(50 * sizeof(char));
    snprintf(main_filename, 50, "%s/[main].html", templates_directory);
    mkdir(temp_directory, 0700);
    mkdir(out_directory, 0700);
    FILE* f = fopen(main_filename, "r");
    char* temp_index_path = malloc(50 * sizeof(char));
    snprintf(temp_index_path, 50, "%s/%s", out_directory, html_file);
    FILE* f2 = fopen(temp_index_path, "w");
    FILE* temp;
    while (fgets(line, 150, f) != NULL){
        if(startswith("!?CSSGI", line)){
            printf("CSSGI\n");
            struct word* lineList;
            lineList = malloc(30 * sizeof(struct word));
            parse_line(line, lineList);
            if (strcmp("FOR", lineList[1].str)==0){
                printf("FOR\n");
                for_mode = true;
            } else if(strcmp("ENDFOR", lineList[1].str)==0){
                printf("ENDFOR\n");
                for_mode = false;
            }
            free(lineList);
        } else if (startswith("!?CSSG", line)){
            struct word* lineList;
            lineList = malloc(30 * sizeof(struct word));
            parse_line(line, lineList);
            int a = 0;
            printf("lineList[%d] : %s\n", a, lineList[a].str);
            int lineListLength = strlen(lineList[1].str);
            char* path = malloc((strlen(lineList[1].str) + strlen(templates_directory)  + 1) * sizeof(char));
            snprintf(path, strlen(templates_directory) + 1 + strlen(lineList[1].str) + strlen(".html") + 1, "%s/%s.html", templates_directory, lineList[1].str);
            if (strcmp(path, main_filename) == 0){
                printf("ERROR : Can't insert [main].html as a template\n");
                exit(1);
            }
            char* temp_path = malloc(40 * sizeof(char));
            snprintf(temp_path, sizeof(temp_path), "%s/nav.html", templates_directory);
            printf("path : %s\n", temp_path);
            free(temp_path);
            temp = fopen(path, "r");
            if (temp == NULL){
                printf("ERROR : Could not open file %s\n", path);
                printf("errno : %d\n", errno);
                exit(1);
            }
            free(path);
            char templine[150];
            while (fgets(templine, 150, temp) != NULL){
                printf("templine : %s\n", templine);
                fprintf(f2, "%s", templine);
            }
            fclose(temp);
            free(lineList);
        } else {
            if (for_mode == true){
                // pass file and add loop to do the mutiple files
                extract_variables(f2, line);

            } else {
            fprintf(f2, line);
            }
        }
    }
    fclose(f);
    fclose(f2);
    free(main_filename);
}