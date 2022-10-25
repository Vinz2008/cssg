#include "utils_file.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

bool is_dir(const char* path){
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}

char* remove_file_extension(const char* filename){
    char* temp = malloc(strlen(filename));
    memset(temp, 0, sizeof(temp));
    for (int i = 0; i < strlen(filename); i++){
        if (filename[i] != '.'){
            strncat(temp, filename + i, 1);
        } else {
            break;
        }
    }
    return temp;
}