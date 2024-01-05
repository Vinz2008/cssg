#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <ftw.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include "parser_config.h"

#ifdef _WIN32
#include "windows.h"
#endif

char** separate_with_space(char* s){
    char** ret = NULL;
    char* s_copy = strdup(s);
    char* ptr = strtok(s_copy, " ");
    int n_spaces = 0;
    while (ptr){
        ret = realloc (ret, sizeof (char*) * ++n_spaces);
        if (ret == NULL)
            exit (-1);
        ret[n_spaces-1] = ptr;

        ptr = strtok (NULL, " ");
    }
    ret = realloc (ret, sizeof (char*) * (n_spaces+1));
    ret[n_spaces] = NULL;
    return ret;
}

int run_command(char* command){
    return system(command);
    /*char** args = separate_with_space(command);
    int dataPipe[2];
    pipe(dataPipe);
    int status;
    char** env = NULL;
    extern char** environ;
    int i = 0;
    while(environ[i]) {
        env = realloc(env, (i+1) * sizeof(char*));
        env[i] = environ[i];
        i++;
    }
    env[i] = NULL; 
    pid_t ret;
    pid_t pid = fork();
    if (pid == -1){
        fprintf(stderr, "error forking\n");
        exit(1);
    } else if (pid != 0){
        ret = waitpid(pid, &status, 0);
        if (ret == -1){
            fprintf(stderr, "error in child process : exited with a failure\n");
            exit(1);
        }
        //while ((ret = waitpid(pid, &status, 0)) == -1) {
        //if (errno != EINTR) {
        //    fprintf(stderr, "error in child process : errno not EINTR\n");
        //    exit(1);
        //}
        //}
        //if ((ret == 0) || !(WIFEXITED(status) && !WEXITSTATUS(status))) {
        //    fprintf(stderr, "error in child process : exited with a failure\n");
        //    exit(1);
        //}
    } else {
        close(dataPipe[0]);
        dup2(1, dataPipe[1]);
        close(dataPipe[1]);
        if (execve(args[0], args, env) == -1) {
            // Handle error
            fprintf(stderr, "error in child process : could not exec the command\n");
            _Exit(127);
        }
    }
    free(args[0]); // free the first args pointer, which is a pointer to s_copy
    free(args);
    free(env);
    return 0;*/
}

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
    strncat(out, folder, strlen(path)-strlen(out)-strlen("/"));
}

void mkdir_if_not_exist(char* path){
    struct stat st = {0};
    printf("create folder : %s\n", path);
    if (stat(path, &st) == -1){
        mkdir(path, 0700);
    }
}