#include "templates.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <mkdio.h>
#include "parser.h"
#include "parser_config.h"
#include "utils.h"
#include "utils_file.h"
#include "misc.h"
#include "config.h"

#ifdef _WIN32
#include "windows.h"
#endif

bool for_mode = false;


void init_file_array(struct file_array* File_array, size_t initialSize){
    File_array->files = malloc(initialSize * sizeof(struct file));
    File_array->used = 0;
    File_array->size = initialSize;
}

void append_file_array(struct file File, struct file_array* File_array){
    if (File_array->used == File_array->size){
        File_array->size *= 2;
        File_array->files = realloc(File_array->files, File_array->size * sizeof(struct file));
    }
    File_array->files[File_array->used++] = File;
}

void destroy_file_array(struct file_array* File_array){
    for (int i = 0; i < File_array->used; i++){
        free(File_array->files[i].name);
        free(File_array->files[i].path);
    }
    free(File_array->files);
    free(File_array);
}


void init_line_array(struct line_array* Line_array, size_t initialSize){
    printf("initialSize : %ld\n", initialSize);
    Line_array->lines = malloc(initialSize * sizeof(struct line));
    printf("TEST\n");
    Line_array->used = 0;
    Line_array->size = initialSize;
}

void append_line_array(struct line Line, struct line_array* Line_array){
    if (Line_array->used == Line_array->size){
        Line_array->size *= 2;
        Line_array->lines = realloc(Line_array->lines, Line_array->size * sizeof(struct line));
    }
    Line_array->lines[Line_array->used++] = Line;
}

void empty_line_array(struct line_array* Line_array){
    free(Line_array->lines);
    Line_array->lines = NULL;
    Line_array->used = Line_array->size = 0;
}

int recurse_nb = 0;
struct file_array* temp_file_array;
struct line_array* Line_array;


void get_file_array(char* directory, char* html_folder){
    //char path[1000];
	char* path = malloc(sizeof(char) * LINE_NB_MAX);
    //memset(path, 0, sizeof(path));
    printf("recurse_nb = %d\n", recurse_nb);
    if (recurse_nb == 0){
        temp_file_array = malloc(sizeof(struct file_array));
        init_file_array(temp_file_array, 1);
    }
    struct dirent *dp;
    DIR *dir = opendir(directory);
	if (!dir){
        free(path);
		return;
	}
	 while ((dp = readdir(dir)) != NULL){
        char* folder = dp->d_name;
        if (strcmp(folder, ".") != 0 && strcmp(folder, "..") != 0){
            printf("file array search file %s\n", folder);
            go_to_folder(folder, directory, path);
            printf("path test : %s\n", path);
			if (is_dir(path)){
                //char temp_html_folder[1000];
                char* temp_html_folder = malloc(sizeof(char) * LINE_NB_MAX);
                go_to_folder(folder, html_folder, temp_html_folder);
                printf("name search file array %s\n", folder);
                printf("temp_file_array->used : %ld\n", temp_file_array->used);
                printf("temp_html_folder : %s\n", temp_html_folder);
                printf("html_folder : %s\n", html_folder);
                //mkdir_if_not_exist(temp_html_folder);
                recurse_nb++;
                get_file_array(path, temp_html_folder);
                free(temp_html_folder);
			} else {
                struct file* temp_file = malloc(sizeof(struct file));
				printf("path for file array : %s\n", path);
                char* temp = remove_file_extension(folder);
                char* html_path = malloc(sizeof(char) * 1000);
				memset(html_path, 0, 1000);
                go_to_folder(temp, html_folder, html_path);
                free(temp);
				strcat(html_path, ".html");
                printf("html path for file array : %s\n", html_path);
                temp_file->path = html_path;
                temp_file->name = remove_file_extension(folder);
                printf("temp_file_array->used before appending : %ld\n", temp_file_array->used);
                append_file_array(*temp_file, temp_file_array);
                printf("path after appending : %s\n", temp_file_array->files[temp_file_array->used - 1].path);
                printf("temp_file_array->used : %ld\n", temp_file_array->used);
                //free(temp_file->path);
                free(temp_file);
			}
            // Construct new path from our base path
        }
    }
    free(path);
    closedir(dir);
    recurse_nb = 0;
}

void log_file_array(const char* log_filename){
    FILE* f = fopen(log_filename, "w");
    fprintf(f, "%ld files\n", temp_file_array->used);
    for (int i = 0; i < temp_file_array->used; i++){
        fprintf(f, "file %d : %s, path : %s\n", i, temp_file_array->files[i].name, temp_file_array->files[i].path);
    }
    fclose(f);
}


void extract_variables(FILE* f, char* line, struct file File, config_t* config){
    int pos = 0;
    for (int i = 0; i < strlen(line);i++){
        if (line[i] == '{' && line[i+1] == '{'){
            pos = i+2;
            printf("TEST\n");
            char* out = malloc(sizeof(char) * 40);
            memset(out, 0, sizeof(char) * 40);
            int pos2 = pos;
            for (int j = 0; j < strlen(line); j++){
            if (line[pos2] == '}' && line[pos2+1] == '}'){
                break;
            }
            out[j] = line[pos2];
            pos2++;
            }
            printf("OUT : %s\n", out);
            if (startswith("filename", out) == 1){
                printf("FILENAME\n");
                char* filename = File.name;
                /*FILE* markdown_file_read = fopen(File.path, "r");
                mkd_flag_t* flags = mkd_flags();
                mkd_set_flag_num(flags, MKD_LATEX);
                mkd_set_flag_num(flags, MKD_FENCEDCODE);
                mkd_set_flag_num(flags, MKD_AUTOLINK);
                MMIOT* md_doc = mkd_in(markdown_file_read, flags);
                char* title = mkd_doc_title(md_doc);
                printf("filename from %% title : %s\n", title);
                fclose(markdown_file_read);*/
                char* title = NULL;
                if (title != NULL){
                    filename = title;
                } else {
                if (config->alias_file != NULL){
                int pos = find_parameter_pos_no_error(File.name, config->alias_file);
                if (pos != -1){
                    filename = config->alias_file->parameters[pos].value_str;
                }
                }
                }
                fprintf(f, "%s", filename);
                pos+=strlen(out) + 3;
                i+=strlen(out) + 3;
            } 
            //printf("startswith(\"path\", %s) %d\n", out, startswith("path", out));
            if (startswith("path", out) == 1){
                printf("PATH\n");
                fprintf(f, "%s", File.path);
                pos+=strlen(out) + 3;
                i+=strlen(out) + 3;
            }
            free(out);
            //break;
        } else {
        fputc(line[i], f);
        }
        }
        /*for (int i = pos; i < strlen(line); i++){
            fputc(line[i], f);
        }*/
}


void extract_variable_files(FILE* f, struct line_array* larray, config_t* config){
    get_file_array(config->articles_directory, ".");
    log_file_array("log.txt");
    for (int i = 0; i < temp_file_array->used; i++){
        printf("path file array [%d] : %s\n", i, temp_file_array->files[i].path);
        for (int j = 0; j < larray->used; j++){
            printf("line in loop : %s\n", larray->lines[j].line);
            extract_variables(f, larray->lines[j].line, temp_file_array->files[i], config);
        }
    }
}


void insert_template(const char* html_file, config_t* config){
    char line[150];
    //FILE* f = fopen(argv[1], "r");
    char* main_filename = malloc(50 * sizeof(char));
    snprintf(main_filename, 50, "%s/[main].html", config->templates_directory);
    mkdir(config->temp_directory, 0700);
    mkdir(config->out_directory, 0700);
    FILE* f = fopen(main_filename, "r");
    if (!f){
        fprintf(stderr, "Couldn't open the main file %s\n", main_filename);
        exit(1);
    }


    char* temp_index_path = malloc(50 * sizeof(char));
    snprintf(temp_index_path, 50, "%s/%s", config->out_directory, html_file);
    FILE* f2 = fopen(temp_index_path, "w");
    free(temp_index_path);
    FILE* temp;
    while (fgets(line, 150, f) != NULL){
        if(startswith("!?CSSGI", line)){
            printf("CSSGI\n");
            struct word* lineList;
            lineList = malloc(30 * sizeof(struct word));
            int lineListLength = parse_line(line, lineList);
            if (strcmp("FOR", lineList[1].str)==0){
                printf("FOR\n");
                for_mode = true;
                Line_array = malloc(sizeof(struct line_array));
                init_line_array(Line_array, 1);
            } else if(strcmp("ENDFOR", lineList[1].str)==0){
                printf("ENDFOR\n");
                for_mode = false;
                extract_variable_files(f2, Line_array, config);
                empty_line_array(Line_array);
                if (Line_array){
                    free(Line_array);
                }
            }
            for (int i = 0; i < lineListLength; i++){
                free(lineList[i].str);
            }
            free(lineList);
        } else if (startswith("!?CSSG", line)){
            struct word* lineList;
            lineList = malloc(30 * sizeof(struct word));
            int lineListLength = parse_line(line, lineList);
            int a = 0;
            printf("lineList[%d] : %s\n", a, lineList[a].str);
            size_t path_length = strlen(lineList[1].str) + strlen(config->templates_directory)  + 1 + strlen(".html") + 1;
            char* path = malloc(path_length * sizeof(char));
            snprintf(path, path_length, "%s/%s.html", config->templates_directory, lineList[1].str);
            if (strcmp(path, main_filename) == 0){
                printf("ERROR : Can't insert [main].html as a template\n");
                exit(1);
            }
            char* temp_path = malloc(40 * sizeof(char));
            snprintf(temp_path, 40, "%s/nav.html", config->templates_directory);
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
            for (int i = 0; i < lineListLength; i++){
                free(lineList[i].str);
            }
            free(lineList);
        } else {
            if (for_mode == true){
                // pass file and add loop to do the mutiple files
                //extract_variable_files(f2, line);
                struct line temp_line;
                //temp_line.line = malloc(strlen(line) * sizeof(char));
                //temp_line.line = line;
                strcpy(temp_line.line, line);
                printf("line after strcpy : %s\n", temp_line.line);
                printf("added line : %s\n", line);
                append_line_array(temp_line, Line_array);
                printf("line after added : %s\n", Line_array->lines[Line_array->used - 1].line);

            } else {
            fprintf(f2, "%s", line);
            }
        }
    }
    fclose(f);
    fclose(f2);
    free(main_filename);
    destroy_file_array(temp_file_array);
}