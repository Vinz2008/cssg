#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "parser.h"
#include "img.h"
#include "templates.h"
#include "parser_config.h"
#include "markdown_converter.h"
#include "misc.h"
#include "libs/startswith.h"

int main(int argc, char **argv){
    if (argc < 2){
        printf("Usage : \n");
        printf("build : build project\n");
        exit(1);
    }
    for (int i = 0; i < argc; i++){
        printf("argv[%d] : %s\n",i, argv[i]);
    }
    struct config_file* root_parameter_file =  parse_config_file("cssg.conf");
    char* temp_directory = root_parameter_file->parameters[find_parameter_pos("temp_directory", root_parameter_file)].value_str;
    char* article_directory = root_parameter_file->parameters[find_parameter_pos("articles_directory", root_parameter_file)].value_str;
    char* templates_directory = root_parameter_file->parameters[find_parameter_pos("templates_directory", root_parameter_file)].value_str;
    char* out_directory = root_parameter_file->parameters[find_parameter_pos("out_directory", root_parameter_file)].value_str;
    char* img_directory = root_parameter_file->parameters[find_parameter_pos("img_directory", root_parameter_file)].value_str;
    if (strcmp(argv[1], "build") == 0){
    insert_template("index.html", root_parameter_file);
    //generate_html_files_recursive("./articles", "./temp");
    printf("pos : %d\n",find_parameter_pos("articles_directory", root_parameter_file));
    printf("article_directory : %s\n", article_directory);
    DIR* dir = opendir(article_directory);
    if (!dir && ENOENT == errno) {
        printf("ERROR : directory %s doesn't exist\n", article_directory);
        exit(1);
    }
    generate_html_files_recursive(article_directory , temp_directory);
    insert_generated_html_in_default_template_recursive(temp_directory, out_directory, root_parameter_file);
    char* img_out_directory = malloc(sizeof(char) * 30);
    snprintf(img_out_directory, 34,"%s/img", out_directory);
    printf("img_out_directory after created : %s\n", img_out_directory);
    mkdir(img_out_directory, 0700);
    copy_img_files(img_directory, img_out_directory);
    } else if (strcmp(argv[1], "clean") == 0){
        clean(root_parameter_file);
    } else {
        printf("ERROR : instruction doesn't exist\n");
        exit(1);
    }
    return 0;
}