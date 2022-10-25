#include <mkdio.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "libs/utils_file.h"

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


