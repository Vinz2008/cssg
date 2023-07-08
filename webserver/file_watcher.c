#include <pthread.h>
#include <stdio.h>


// will maybe use it if there are more than 2 args
struct file_watcher_arg {
    char* src_folder;
};

void* file_watcher(void* arg){
    char* src_folder = (char*)arg;
    printf("test hello from file watcher\n");
    return NULL;
}

void create_file_watcher(char* src_folder){
    pthread_t thr;
    int i = 0;
    pthread_create( &thr, NULL, file_watcher, (void*)src_folder);
    pthread_join(thr, NULL);
}