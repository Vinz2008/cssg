#include "utils_file.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
  #define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif


bool is_dir(const char* path){
#ifdef _WIN32
    struct _stat statbuf;
    if (_stat(path, &statbuf) != 0)
#else
    struct stat statbuf;
    if (stat(path, &statbuf) != 0)
#endif
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