#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include "libs/utils_file.h"

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
    char path[1000];
	memset(path, 0, sizeof(path));
    struct dirent *dp;
    DIR *dir = opendir(img_folder);
	if (!dir){
		return;
	}
	 while ((dp = readdir(dir)) != NULL){
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0){
            printf("%s\n", dp->d_name);
            // Construct new path from our base path
            strcpy(path, img_folder);
            strcat(path, "/");
            strcat(path, dp->d_name);
			if (is_dir(path)){
				char temp_html_folder[1000];
				strcpy(temp_html_folder, img_folder);
            	strcat(temp_html_folder, "/");
            	strcat(temp_html_folder, dp->d_name);
				struct stat st = {0};
				if (stat(temp_html_folder, &st) == -1) {
    			mkdir(temp_html_folder, 0700);
				}
            	copy_img_files(path, img_out_folder);
			} else {
				char img_path[1000];
				memset(img_path, 0, sizeof(img_path));
				strcpy(img_path, img_out_folder);
            	strcat(img_path, "/");
				strcat(img_path, dp->d_name);
				printf("out : %s\n", img_path);
                copy_binary_file(path, img_path);
				
			}
        }
    }
    closedir(dir);
}