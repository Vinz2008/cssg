#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
#include <string.h>
#include "parser_config.h"


int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf){
    int rv = remove(fpath);
    if (rv)
        perror(fpath);
    return rv;
}

int rmrf(char *path){
    return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}

void clean(struct config_file* config){
    rmrf(config->parameters[find_parameter_pos("out_directory", config)].value_str);
    rmrf(config->parameters[find_parameter_pos("temp_directory", config)].value_str);
}

void go_to_folder(char* folder, char* path, char* out){
    strcpy(out, path);
    strcat(out, "/");
    strcat(out, folder);
}