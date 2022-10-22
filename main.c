#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "config.h"
#include "parser.h"
#include "parser_config.h"
#include "libs/startswith.h"

int main(int argc, char **argv){
    struct parameter_file* root_parameter_file =  parse_config_file("cssg.conf");
    char line[150];
    FILE* f = fopen(argv[1], "r");
    FILE* f2 = fopen("index.html", "w");
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
            snprintf(path, strlen(templates_directory) + 1 + strlen(lineList[1].str) + strlen(".html") + 1, templates_directory "%s.html", lineList[1].str);
            printf("path : %s\n", templates_directory "nav.html");
            temp = fopen(path, "r");
            if (temp == NULL){
                printf("ERROR : Could not open file %s\n", path);
                printf("errno : %d\n", errno);
                exit(1);
            }
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
    return 0;
}