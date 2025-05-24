#include "markdown_converter.h"
#include <mkdio.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include "parser_config.h"
#include "parser.h"
#include "utils_file.h"
#include "utils.h"
#include "misc.h"
#include "config.h"
#include "syntax-highlighting.h"

#define BUF_LINE_SIZE 4096

void convert_markdown_to_html(char* md_path, char* html_path, config_t* config){
	printf("in path convert to markdown : %s\n", md_path);
	FILE* in = fopen(md_path, "r");
	FILE* out = fopen(html_path, "w");
    mkd_flag_t* flags = mkd_flags();
    //mkd_set_flag_bitmap(flags, MKD_LATEX | MKD_FENCEDCODE | MKD_AUTOLINK);
	mkd_set_flag_num(flags, MKD_LATEX);
    mkd_set_flag_num(flags, MKD_FENCEDCODE);
    mkd_set_flag_num(flags, MKD_AUTOLINK);
    //int flags = MKD_LATEX | MKD_FENCEDCODE | MKD_AUTOLINK;
	MMIOT* mmiot = mkd_in(in, flags);
    init_syntax_highlighting(mmiot, config);
	markdown(mmiot, out, flags);
	fclose(in);
	fclose(out);
    free(flags);
}


void generate_html_files_recursive(char* article_folder, char* html_folder, config_t* config){
	//char path[1000];
	char* path = malloc(sizeof(char) * LINE_NB_MAX);
    //memset(path, 0, sizeof(path));
    struct dirent *dp;
    DIR *dir = opendir(article_folder);
	if (!dir){
        free(path);
		return;
	}
	 while ((dp = readdir(dir)) != NULL){
		char* folder = dp->d_name;
        if (strcmp(folder, ".") != 0 && strcmp(folder, "..") != 0){
            printf("%s\n", folder);
            // Construct new path from our base path
			go_to_folder(folder, article_folder, path);
			if (is_dir(path)){
				//char temp_html_folder[1000];
				char* temp_html_folder = malloc(sizeof(char) * LINE_NB_MAX);
                go_to_folder(folder, html_folder, temp_html_folder);
				mkdir_if_not_exist(temp_html_folder);
            	generate_html_files_recursive(path, temp_html_folder, config);
                free(temp_html_folder);
            } else {
				//char html_path[1000];
				char* html_path = malloc(sizeof(char) * LINE_NB_MAX);
                //memset(html_path, 0, sizeof(html_path));
				char* name_extension_removed = remove_file_extension(folder);
				printf("file extension removed %s\n", name_extension_removed);
				go_to_folder(name_extension_removed, html_folder, html_path);
				free(name_extension_removed);
				strcat(html_path, ".html");
				printf("out : %s\n", html_path);
				printf("in : %s\n", path);
				convert_markdown_to_html(path, html_path, config);
                free(html_path);
			}
        }
    }
    free(path);
    closedir(dir);
}

void insert_in_default_template(char* filename, config_t* config, char* out_path){
	char line[BUF_LINE_SIZE];
	FILE* out = fopen(out_path, "w");
	char* default_filename = malloc(50 * sizeof(char));
	snprintf(default_filename, 50, "%s/[default].html", config->templates_directory);
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
	struct word* lineList;
	printf("content file : %s\n", filename);
	printf("default file : %s\n", default_filename);
	free(default_filename);
	printf("out file : %s\n", out_path);
	while (fgets(line, BUF_LINE_SIZE, default_file) != NULL){
		if (startswith("!?CSSG", line)){
            lineList = malloc(30 * sizeof(struct word));
            int lineListSize = parse_line(line, lineList);
			if (strcmp(lineList[1].str, "CONTENT") == 0){
				printf("CONTENT\n");
				char line2[BUF_LINE_SIZE];
				while (fgets(line2, BUF_LINE_SIZE, in) != NULL){
					fprintf(out, "%s\n", line2);
				}
			} else {
				char* temp_path = malloc(40 * sizeof(char));
				snprintf(temp_path, 40, "%s/%s.html", config->templates_directory, lineList[1].str);
				printf("temp_path : %s\n", temp_path);
				temp = fopen(temp_path, "r");
				char line3[BUF_LINE_SIZE];
				printf("TEST\n");
				while (fgets(line3, BUF_LINE_SIZE, temp) != NULL){
					printf("line %s : %s\n", temp_path, line3);
					fprintf(out, "%s", line3);
				}
                fprintf(out, "\n");
				printf("TEST2\n");
				fclose(temp);
				free(temp_path);
			}
            for (int i = 0; i < lineListSize; i++){
                free(lineList[i].str);
            }
			free(lineList);
		} else {
			printf("line [default] : %s\n", line);
			fprintf(out, "%s", line);
		}
	}
	fclose(in);
	fclose(default_file);
	fclose(out);

}

void insert_generated_html_in_default_template_recursive(char* temp_folder, char* html_folder, config_t* config){
	//char path[1000];
	char* path = malloc(sizeof(char) * LINE_NB_MAX);
    //memset(path, 0, sizeof(path));
    struct dirent *dp;
    DIR *dir = opendir(temp_folder);
	if (!dir){
        free(path);
		return;
	}
	 while ((dp = readdir(dir)) != NULL){
		char* folder = dp->d_name;
        if (strcmp(folder, ".") != 0 && strcmp(folder, "..") != 0){
            printf("%s\n", folder);
            // Construct new path from our base path
			go_to_folder(folder, temp_folder, path);
			if (is_dir(path)){
				//char temp_html_folder[1000];
				char* temp_html_folder = malloc(sizeof(char) * LINE_NB_MAX);
                go_to_folder(folder, html_folder, temp_html_folder);
				mkdir_if_not_exist(temp_html_folder);
            	insert_generated_html_in_default_template_recursive(path ,temp_html_folder, config);
                free(temp_html_folder);
            } else {
				char* name_extension_removed = remove_file_extension(folder);
				printf("file extension removed %s\n", name_extension_removed);
				//char html_path[1000];
				char* html_path = malloc(sizeof(char) * LINE_NB_MAX);
                //memset(html_path, 0, sizeof(html_path));
				go_to_folder(name_extension_removed, html_folder, html_path);
				strcat(html_path, ".html");
				printf("out : %s\n", html_path);
				insert_in_default_template(path,  config, html_path);
				free(name_extension_removed);
                free(html_path);
			}
        }
    }
    free(path);
    closedir(dir);
}

