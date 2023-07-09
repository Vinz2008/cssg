#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

#include "../misc.h"
#include "../libs/utils_file.h"

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
    return buf.st_mtime;
}

void getFileListRecurse(struct FileList* fl, char* current_folder, char* path){
    memset(path, 0, CUSTOM_PATH_MAX);
    struct dirent *dp;
    DIR *dir = opendir(current_folder);
	if (!dir){
		return NULL;
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
    struct dirent *dp;
    char* path = malloc(sizeof(char) * CUSTOM_PATH_MAX);
    getFileListRecurse(fl, folder, path);
    free(path);
    return fl;
}


static inline void update_mtime(struct file* f){
    f->mtime = get_mtime(f->path); 
}

bool has_file_changed(struct file* f){
    return get_mtime(f->path) > f->mtime;
}

void* file_watcher(void* arg){
    char* src_folder = (char*)arg;
    struct FileList* filelist = createFileList();
    printf("test hello from file watcher\n");
    getFileList("./articles"); // change to list of folders so it can search in multiple folders
    return NULL;
}

void create_file_watcher(char* src_folder){
    pthread_t thr;
    int i = 0;
    pthread_create( &thr, NULL, file_watcher, (void*)src_folder);
    pthread_join(thr, NULL);
}