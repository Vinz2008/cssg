#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include "parser.h"
#include "parser_config.h"
#include "markdown_converter.h"
#include "libs/startswith.h"

int main(int argc, char **argv){
    struct config_file* root_parameter_file =  parse_config_file("cssg.conf");
    char* temp_directory = root_parameter_file->parameters[find_parameter_pos("temp_directory", root_parameter_file)].value_str;
    char* article_directory = root_parameter_file->parameters[find_parameter_pos("articles_directory", root_parameter_file)].value_str;
    char* templates_directory = root_parameter_file->parameters[find_parameter_pos("templates_directory", root_parameter_file)].value_str;
    char* out_directory = root_parameter_file->parameters[find_parameter_pos("out_directory", root_parameter_file)].value_str;
    char line[150];
    //FILE* f = fopen(argv[1], "r");
    char* main_filename = malloc(50 * sizeof(char));
    snprintf(main_filename, 50, "%s/[main].html", templates_directory);
    FILE* f = fopen(main_filename, "r");
    FILE* f2 = fopen("out/index.html", "w");
    FILE* temp;
    while (fgets(line, 150, f) != NULL){
        if (startswith("!?CSSG", line)){
            struct word* lineList;
            lineList = malloc(30 * sizeof(struct word));
            parse_line(line, lineList);
            int a = 0;
            printf("lineList[%d] : %s\n", a, lineList[a].str);
            int lineListLength = strlen(lineList[1].str);
            char* path = malloc((strlen(lineList[1].str) + strlen(templates_directory)  + 1) * sizeof(char));
            snprintf(path, strlen(templates_directory) + 1 + strlen(lineList[1].str) + strlen(".html") + 1, "%s/%s.html", templates_directory, lineList[1].str);
            if (strcmp(path, main_filename) == 0){
                printf("ERROR : Can't insert [main].html as a template\n");
                exit(1);
            }
            char* temp_path = malloc(40 * sizeof(char));
            snprintf(temp_path, sizeof(temp_path), "%s/nav.html", templates_directory);
            printf("path : %s\n", temp_path);
            free(temp_path);
            temp = fopen(path, "r");
            if (temp == NULL){
                printf("ERROR : Could not open file %s\n", path);
                printf("errno : %d\n", errno);
                exit(1);
            }
            free(path);
            char templine[150];
            while (fgets(templine, 150, temp) != NULL){
                printf("templine : %s\n", templine);
                fprintf(f2, "%s\n", templine);
            }
            fclose(temp);
            free(lineList);

        } else {
            fprintf(f2, line);
        }
    }
    fclose(f);
    fclose(f2);
    mkdir(temp_directory, 0700);
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
    free(main_filename);
    return 0;
}