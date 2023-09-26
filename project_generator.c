#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include "parser_config.h"

#ifdef _WIN32
#include "windows.h"
#endif

char* cssg_conf_template = "templates_directory=./templates\n"
"img_directory=./img\n"
"articles_directory=./articles\n"
"temp_directory=./temp\n"
"out_directory=./out\n";


void generate_project(char* project_name){
    mkdir(project_name, 0700);
    char* cssg_conf_name  = "cssg.conf";
    char* cssg_conf_path = malloc((strlen(project_name) + strlen(cssg_conf_name) + 1) * sizeof(char));
    sprintf(cssg_conf_path, "%s/%s", project_name, cssg_conf_name);
    FILE* cssg_conf_f = fopen(cssg_conf_path, "w");
    fprintf(cssg_conf_f, "%s", cssg_conf_template);
    fclose(cssg_conf_f);
    char* articles_folder_name = "articles";
    char* articles_folder = malloc((strlen(project_name) + strlen(articles_folder_name) + 1) * sizeof(char));
    sprintf(articles_folder, "%s/%s", project_name, articles_folder_name);
    mkdir(articles_folder, 0700);
    char* templates_folder_name = "templates";
    char* templates_folder = malloc((strlen(project_name) + strlen(templates_folder_name) + 1) * sizeof(char));
    sprintf(templates_folder, "%s/%s", project_name, templates_folder_name);
    mkdir(templates_folder, 0700);
    free(cssg_conf_path);
    free(articles_folder);
    free(templates_folder);
}