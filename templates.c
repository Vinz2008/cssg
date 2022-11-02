#include "templates.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include "parser.h"
#include "parser_config.h"
#include "libs/startswith.h"
#include "libs/utils_file.h"

bool for_mode = false;

void init_file_array(struct file_array* File_array, size_t initialSize){
    File_array->files = malloc(initialSize * sizeof(struct file));
    File_array->used = 0;
    File_array->size = initialSize;
}

void append_file_array(struct file File, struct file_array* File_array){
    if (File_array->used == File_array->size){
        File_array->size *= 2;
        File_array->files = realloc(File_array->files, File_array->size * sizeof(struct parameter));
    }
    File_array->files[File_array->used++] = File;
}

int recurse_nb = 0;
struct file_array* temp_file_array;

void get_file_array(char* directory){
    char path[1000];
	memset(path, 0, sizeof(path));
    if (recurse_nb == 0){
        temp_file_array = malloc(sizeof(struct file_array));
        init_file_array(temp_file_array, 1);
    }
    struct dirent *dp;
    DIR *dir = opendir(directory);
	if (!dir){
		return;
	}
	 while ((dp = readdir(dir)) != NULL){
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            printf("name search file array %s\n", dp->d_name);
            strcpy(path, directory);
            strcat(path, "/");
            strcat(path, dp->d_name);
            printf("path test : %s\n", path);
			if (is_dir(path)){
                get_file_array(path);
			} else {
                struct file* temp_file = malloc(sizeof(struct file));
				printf("path for file array : %s\n", path);
                temp_file->path = path;
                temp_file->name = remove_file_extension(dp->d_name);
                append_file_array(*temp_file, temp_file_array);
                //free(temp_file);
			}
            // Construct new path from our base path
            
        }
    }
    closedir(dir);
    recurse_nb = 0;
}



void extract_variables(FILE* f, char* line, struct file File){
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
                fprintf(f, "%s", File.path);
                pos+=strlen(out) + 3;
                i+=strlen(out) + 3;
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


void extract_variable_files(FILE* f, char* line){
    get_file_array("./articles");
    for (int i = 0; i < temp_file_array->used; i++){
        printf("path file array [%d] : %s\n", i, temp_file_array->files[i].path);
        extract_variables(f, line, temp_file_array->files[i]);
    }
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
                extract_variable_files(f2, line);

            } else {
            fprintf(f2, line);
            }
        }
    }
    fclose(f);
    fclose(f2);
    free(main_filename);
}