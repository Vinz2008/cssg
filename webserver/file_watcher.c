#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#define _POSIX1_SOURCE 2
#include <unistd.h>

#ifdef _WIN32
#include <libloaderapi.h>
#endif

#include "../src/misc.h"
#include "../libs/utils_file.h"
#include "webserver.h"

// will maybe use it if there are more than 2 args
struct file_watcher_arg {
    char* src_folder;
};

struct file {
    char* path;
    long mtime;
};

#define FILELIST_FACTOR 3
#define CUSTOM_PATH_MAX 1000


struct FileList {
    struct file* list;
    size_t allocated_size;
    size_t length;
};

char* get_self_path(){
    char* self_path = malloc(sizeof(char) * CUSTOM_PATH_MAX);
#ifndef _WIN32
    readlink("/proc/self/exe", self_path, CUSTOM_PATH_MAX);
#else
    GetModuleFileNameA(NULL, self_path, CUSTOM_PATH_MAX);
#endif
    return self_path;
}

void rebuild_folder(){
    char* self_path = get_self_path();
    char* format = "%s build";
    size_t cmd_size = (CUSTOM_PATH_MAX + strlen(format)) * sizeof(char);
    char* cmd = malloc(cmd_size);
    snprintf(cmd, cmd_size, format, self_path);
    printf("cmd : %s\n", cmd);
    run_command(cmd);
    free(cmd);
    free(self_path);
}

struct FileList* createFileList(){
    struct FileList* fl = malloc(sizeof(struct FileList));
    fl->list = malloc(sizeof(struct file) * FILELIST_FACTOR); 
    fl->length = 0;
    fl->allocated_size = FILELIST_FACTOR;
    return fl;
}

void appendFileList(struct file f, struct FileList* fl){
    if (fl->length == fl->allocated_size){
        fl->allocated_size += FILELIST_FACTOR;
        fl->list = realloc(fl->list, fl->allocated_size * sizeof(struct file));
    }
    fl->list[fl->length++] = f; 
}

void destroyFileList(struct FileList* fl){
    free(fl->list);
    free(fl);
}

long get_mtime(char* path){
    struct stat buf;
    memset(&buf, 0, sizeof(buf));
    stat(path, &buf);
    //printf("got mtime %s : %ld\n", path, buf.st_mtime);
    return buf.st_mtime;
}

void getFileListRecurse(struct FileList* fl, char* current_folder, char* path){
    memset(path, 0, CUSTOM_PATH_MAX);
    struct dirent *dp;
    DIR *dir = opendir(current_folder);
	if (!dir){
		return;
	}
    while ((dp = readdir(dir)) != NULL){
        char* folder = dp->d_name;
        if (strcmp(folder, ".") != 0 && strcmp(folder, "..") != 0){
            printf("folder : %s\n", folder);
            go_to_folder(folder, current_folder, path);
            if (is_dir(path)){
                printf("dir : %s\n", path);
                char* temp_new_path = malloc(sizeof(char) * CUSTOM_PATH_MAX);
                //go_to_folder(folder, path, temp_new_path);
                strcpy(temp_new_path, path);
                getFileListRecurse(fl, path, temp_new_path);
                free(temp_new_path);
            } else {
                // create complete path from current folder"
                char* file_path = malloc(sizeof(char) * CUSTOM_PATH_MAX);
                go_to_folder(folder, current_folder, file_path);
                printf("file path added : %s\n", file_path);
                struct file temp_file;
                temp_file.path = file_path;
                temp_file.mtime = get_mtime(file_path);
                appendFileList(temp_file, fl);
            }
        }
    }
    closedir(dir);

}

struct FileList* getFileList(char* folder){
    struct FileList* fl = createFileList();
    char* path = malloc(sizeof(char) * CUSTOM_PATH_MAX);
    getFileListRecurse(fl, folder, path);
    free(path);
    return fl;
}

struct FileList* getFileListNoRecurse(char* current_folder){
    struct FileList* fl = createFileList();
    char* path = malloc(sizeof(char) * CUSTOM_PATH_MAX);
    memset(path, 0, CUSTOM_PATH_MAX);
    struct dirent *dp;
    DIR *dir = opendir(current_folder);
	if (!dir){
		return NULL;
	}

    while ((dp = readdir(dir)) != NULL){
        char* folder = dp->d_name;
        if (strcmp(folder, ".") != 0 && strcmp(folder, "..") != 0){
            if (!is_dir(path)){
                char* file_path = malloc(sizeof(char) * CUSTOM_PATH_MAX);
                go_to_folder(folder, current_folder, file_path);
                printf("file path added : %s\n", file_path);
                struct file temp_file;
                temp_file.path = file_path;
                temp_file.mtime = get_mtime(file_path);
                appendFileList(temp_file, fl);
            }
        }
    }
    closedir(dir);
    free(path);
    return fl;
}


static inline void update_mtime(struct file* f){
    f->mtime = get_mtime(f->path); 
}

bool has_file_changed(struct file* f){
    return get_mtime(f->path) > f->mtime;
}

void append_to_fileList_config_files(struct FileList* filelistToTrack){
    struct FileList* localFileList = getFileListNoRecurse(".");
    for (int i = 0; i < localFileList->length; i++){
        //printf("local files searching config : %s\n", localFileList->list[i].path);
        //printf("local files file extension searching config : %s\n", get_file_extension(localFileList->list[i].path));
        if (strcmp(get_file_extension(localFileList->list[i].path), "conf") == 0){
            printf("found conf file : %s\n", localFileList->list[i].path);
            appendFileList(localFileList->list[i], filelistToTrack);
        }
    }
    destroyFileList(localFileList);
}

void* file_watcher(void* arg){
    //char* src_folder = (char*)arg;
    struct FileList* filelist /*= createFileList()*/;
    printf("test hello from file watcher\n");
    rebuild_folder();
    filelist = getFileList("./articles"); // change to list of folders so it can search in multiple folders
    append_to_fileList_config_files(filelist);
    for (int i = 0; i < filelist->length; i++){
        printf("file %d : %s with mtime %ld\n", i, filelist->list[i].path, filelist->list[i].mtime);
    }
    while(1){
        //printf("watching files\n");
        for (int i = 0; i < filelist->length; i++){
            if (has_file_changed(filelist->list + i)){
                printf("file changed %s\n", filelist->list[i].path);
                rebuild_folder();
                update_mtime(filelist->list + i);

            }
        }
    }
    //return NULL;
}

void create_file_watcher(char* src_folder){
    pthread_t thr;
    pthread_create( &thr, NULL, file_watcher, (void*)src_folder);
    //pthread_join(thr, NULL);
    pthread_detach(thr);
}