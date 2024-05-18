#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include "parser.h"
#include "img.h"
#include "templates.h"
#include "parser_config.h"
#include "markdown_converter.h"
#include "misc.h"
#include "project_generator.h"
#include "../libs/utils_file.h"
#include "../libs/startswith.h"
#include "../webserver/webserver.h"

#ifdef _WIN32
#include "windows.h"
#endif

struct config_file* alias_file = NULL;
struct config_file* root_parameter_file;
config_t* config;

enum run_mode {
    BUILD_MODE,
    SERVE_MODE,
    CLEAN_MODE,
    NOT_FOUND_MODE
};

void atexit_main_webserver(){
    if (alias_file){
        empty_config_list(alias_file);
        free(alias_file);
    }
    empty_config_list(root_parameter_file);
    free(root_parameter_file);
    free(config);
}

int main(int argc, char **argv){
    enum run_mode mode= NOT_FOUND_MODE;
    if (argc < 2 || strcmp(argv[1], "help") == 0){
        printf("Usage : \n");
        printf("build : build project\n");
        printf("clean : clean project\n");
        printf("serve : serve the project\n");
        printf("new [NAME] : create new project\n");
        return 0;
    }
    /*for (int i = 0; i < argc; i++){
        printf("argv[%d] : %s\n",i, argv[i]);
    }*/
    char* start_folder = ".";
    for (int i = 0; i < argc; i++){
    if (strcmp(argv[i], "build") == 0){
        mode = BUILD_MODE;
    } else if (strcmp(argv[i], "clean") == 0){
        mode = CLEAN_MODE;
    } else if (strcmp(argv[i], "serve") == 0){
        mode = SERVE_MODE;;
    } else if (strcmp(argv[i], "-C") == 0){
        i++;
        start_folder = argv[i];
        printf("start folder : %s\n", start_folder);
    }
    }
    if (mode == NOT_FOUND_MODE) {
        printf("ERROR : no instruction given");
        exit(1);
    }
    if (strcmp(argv[1], "new") == 0){
        generate_project(argv[2]);
        return 0;
    }
    char* alias_file_path = get_file_path_in_folder(start_folder, "alias.conf");
    if (if_file_exists(alias_file_path)){
        alias_file = parse_config_file(alias_file_path);
    }
    char* root_file_path = get_file_path_in_folder(start_folder, "cssg.conf");
    root_parameter_file =  parse_config_file(root_file_path);
    char* temp_directory = root_parameter_file->parameters[find_parameter_pos("temp_directory", root_parameter_file)].value_str;
    char* article_directory = root_parameter_file->parameters[find_parameter_pos("articles_directory", root_parameter_file)].value_str;
    char* templates_directory = root_parameter_file->parameters[find_parameter_pos("templates_directory", root_parameter_file)].value_str;
    char* out_directory = root_parameter_file->parameters[find_parameter_pos("out_directory", root_parameter_file)].value_str;
    char* img_directory = root_parameter_file->parameters[find_parameter_pos("img_directory", root_parameter_file)].value_str;
    config = malloc(sizeof(config_t));
    config->articles_directory = get_file_path_in_folder(start_folder, article_directory);
    config->img_directory = get_file_path_in_folder(start_folder, img_directory);
    config->out_directory = get_file_path_in_folder(start_folder, out_directory);
    config->temp_directory = get_file_path_in_folder(start_folder, temp_directory);
    config->templates_directory = get_file_path_in_folder(start_folder, templates_directory);
    config->alias_file = alias_file;
    char* highlighting_theme = "nord";
    if (find_parameter_pos_no_error("syntax_highlighting_theme", root_parameter_file) != -1){
        highlighting_theme = root_parameter_file->parameters[find_parameter_pos("syntax_highlighting_theme", root_parameter_file)].value_str;
    }
    config->syntax_config.theme = highlighting_theme;
    if (/*strcmp(argv[1], "build") == 0*/ mode == BUILD_MODE){
    insert_template("index.html", config);
    //generate_html_files_recursive("./articles", "./temp");
    printf("article_directory : %s\n", config->articles_directory);
    DIR* dir = opendir(config->articles_directory);
    if (!dir || ENOENT == errno) {
        printf("ERROR : directory %s doesn't exist\n", config->articles_directory);
        exit(1);
    }
    closedir(dir);
    generate_html_files_recursive(config->articles_directory , config->temp_directory, config);
    insert_generated_html_in_default_template_recursive(config->temp_directory, config->out_directory, config);
    char* img_out_directory = malloc(sizeof(char) * 4096);
    snprintf(img_out_directory, 35,"%s/img", config->out_directory);
    printf("img_out_directory after created : %s\n", img_out_directory);
    mkdir(img_out_directory, 0700);
    copy_img_files(img_directory, img_out_directory);
    free(img_out_directory);
    } else if (/*strcmp(argv[1], "clean") == 0*/ mode == CLEAN_MODE){
        clean(root_parameter_file);
    } else if (/*strcmp(argv[1], "serve") == 0*/ mode == SERVE_MODE){
        char* folder = get_file_path_in_folder(start_folder, out_directory);
        //folder = out_directory;
    
        /*if (argc < 3){
            folder = ".";
        } else {
            folder = argv[2];
        }*/
#ifdef _WIN32
    printf("webserver is not implemented in windows for now\n");
    exit(1);
#else
        atexit(atexit_main_webserver);
        webserver(folder);
#endif
    } else {
        printf("ERROR : instruction doesn't exist\n");
        exit(1);
    }
    empty_config_list(root_parameter_file);
    if (alias_file){
        empty_config_list(alias_file);
        free(alias_file);
    }
    free(root_parameter_file);
    free(config);
    return 0;
}