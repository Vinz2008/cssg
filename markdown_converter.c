#include "markdown_converter.h"
#include <mkdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "parser_config.h"
#include "parser.h"
#include "libs/utils_file.h"
#include "libs/startswith.h"

void convert_markdown_to_html(char* md_path, char* html_path){
	FILE* in = fopen(md_path, "r");
	FILE* out = fopen(html_path, "w");
	MMIOT* mmiot = mkd_in(in, MKD_LATEX);
	markdown(mmiot, out, MKD_LATEX);
	fclose(in);
	fclose(out);
}


void generate_html_files_recursive(char* article_folder, char* html_folder){
	char path[1000];
	memset(path, 0, sizeof(path));
    struct dirent *dp;
    DIR *dir = opendir(article_folder);
	if (!dir){
		return;
	}
	 while ((dp = readdir(dir)) != NULL){
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            printf("%s\n", dp->d_name);
            // Construct new path from our base path
            strcpy(path, article_folder);
            strcat(path, "/");
            strcat(path, dp->d_name);
			if (is_dir(path)){
				char temp_html_folder[1000];
				strcpy(temp_html_folder, html_folder);
            	strcat(temp_html_folder, "/");
            	strcat(temp_html_folder, dp->d_name);
				struct stat st = {0};
				if (stat(temp_html_folder, &st) == -1) {
    			mkdir(temp_html_folder, 0700);
				}
            	generate_html_files_recursive(path, temp_html_folder);
			} else {
				char html_path[1000];
				memset(html_path, 0, sizeof(html_path));
				strcpy(html_path, html_folder);
            	strcat(html_path, "/");
				printf("file extension removed %s\n", remove_file_extension(dp->d_name));
				strcat(html_path, remove_file_extension(dp->d_name));
				strcat(html_path, ".html");
				printf("out : %s\n", html_path);
				convert_markdown_to_html(path, html_path);
			}
        }
    }
    closedir(dir);
}

void insert_in_default_template(char* filename, struct config_file* config, char* out_path){
	char line[1024];
	FILE* out = fopen(out_path, "w");
	char* default_filename = malloc(50 * sizeof(char));
	snprintf(default_filename, 50, "%s/[default].html", config->parameters[find_parameter_pos("templates_directory", config)].value_str);
	FILE* default_file = fopen(default_filename, "r");
	FILE* in = fopen(filename, "r");
	FILE* temp;
	if (in == NULL){
		printf("could not open file %s\n", filename);
		exit(1);
	}
	if (default_file == NULL){
		printf("could not open file %s\n", default_filename);
		exit(1);
	}
	printf("content file : %s\n", filename);
	printf("default file : %s\n", default_filename);
	printf("out file : %s\n", out_path);
	while (fgets(line, 1024, default_file) != NULL){
		if (startswith("!?CSSG", line)){
			struct word* lineList;
            lineList = malloc(30 * sizeof(struct word));
            parse_line(line, lineList);
			if (strcmp(lineList[1].str, "CONTENT") == 0){
				printf("CONTENT\n");
				char line2[1024];
				while (fgets(line2, 1024, in) != NULL){
					fprintf(out, "%s\n", line2);
				}
			} else {
				char* temp_path = malloc(40 * sizeof(char));
				snprintf(temp_path, 40, "%s/%s.html", config->parameters[find_parameter_pos("templates_directory", config)].value_str , lineList[1].str);
				printf("temp_path : %s\n", temp_path);
				temp = fopen(temp_path, "r");
				char line3[1024];
				printf("TEST\n");
				while (fgets(line3, 1024, temp) != NULL){
					printf("line %s : %s\n", temp_path, line3);
					fprintf(out, "%s\n", line3);
				}
				printf("TEST2\n");
				fclose(temp);
				free(temp_path);
			}
		} else {
			printf("line [default] : %s\n", line);
			fprintf(out, "%s\n", line);
		}
	}
	fclose(in);
	fclose(default_file);
	fclose(out);
}

void insert_generated_html_in_default_template_recursive(char* temp_folder, char* html_folder, struct config_file* config){
	char path[1000];
	memset(path, 0, sizeof(path));
    struct dirent *dp;
    DIR *dir = opendir(temp_folder);
	if (!dir){
		return;
	}
	 while ((dp = readdir(dir)) != NULL){
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            printf("%s\n", dp->d_name);
            // Construct new path from our base path
            strcpy(path, temp_folder);
            strcat(path, "/");
            strcat(path, dp->d_name);
			if (is_dir(path)){
				char temp_html_folder[1000];
				strcpy(temp_html_folder, html_folder);
            	strcat(temp_html_folder, "/");
            	strcat(temp_html_folder, dp->d_name);
				struct stat st = {0};
				if (stat(temp_html_folder, &st) == -1) {
    			mkdir(temp_html_folder, 0700);
				}
            	insert_generated_html_in_default_template_recursive(path ,temp_html_folder, config);
			} else {
				char html_path[1000];
				memset(html_path, 0, sizeof(html_path));
				strcpy(html_path, html_folder);
            	strcat(html_path, "/");
				printf("file extension removed %s\n", remove_file_extension(dp->d_name));
				strcat(html_path, remove_file_extension(dp->d_name));
				strcat(html_path, ".html");
				printf("out : %s\n", html_path);
				insert_in_default_template(path,  config, html_path);
				convert_markdown_to_html(path, html_path);
			}
        }
    }
    closedir(dir);
}

