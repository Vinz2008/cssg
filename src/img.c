#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "../libs/utils_file.h"
#include "misc.h"
#include "config.h"

void copy_binary_file(const char* src_path, const char* dest_path){
    FILE* fsrc = fopen(src_path, "rb");
    fseek(fsrc, 0, SEEK_SET);
    FILE* fdest = fopen(dest_path, "wb");
    int c;
    while(true){
      c = fgetc(fsrc);
      if (feof(fsrc)){
         break;
      }
      fputc(c, fdest);
   }
    fclose(fsrc);
    fclose(fdest);
}

void copy_img_files(const char* img_folder, const char* img_out_folder){
    printf("img_out_folder: %s\n", img_out_folder);
    //char path[1000];
	char* path = malloc(sizeof(char) * LINE_NB_MAX);
    //memset(path, 0, sizeof(path));
    struct dirent *dp;
    DIR *dir = opendir(img_folder);
	if (!dir){
        free(path);
		return;
	}
	 while ((dp = readdir(dir)) != NULL){
        char* folder = dp->d_name;
        if (strcmp(folder, ".") != 0 && strcmp(folder, "..") != 0){
            printf("%s\n", folder);
            // Construct new path from our base path
            go_to_folder(folder, (char*)img_folder, path);
			if (is_dir(path)){
				//char temp_html_folder[1000];
                char* temp_html_folder = malloc(sizeof(char) * LINE_NB_MAX);
                go_to_folder(folder, (char*)img_folder,temp_html_folder);
                mkdir_if_not_exist(temp_html_folder);
            	copy_img_files(path, img_out_folder);
			} else {
				//char img_path[1000];
                char* img_path = malloc(sizeof(char) * LINE_NB_MAX);
				//memset(img_path, 0, sizeof(img_path));
                go_to_folder(folder, (char*)img_out_folder,img_path);
				printf("out : %s\n", img_path);
                copy_binary_file(path, img_path);
				free(img_path);
			}
        }
    }
    closedir(dir);
    free(path);
}