#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libs/string.h"
#include "config.h"

int parse_line(char* line, struct word* lineList){
    //char line2[1000];
    char* line2 = malloc(sizeof(char) * LINE_NB_MAX);
    strcpy(line2, line);
    removeCharFromString('\n', line2);
    int i = 0;
    char *p = strtok(line2, " ");
    while(p != NULL){
        printf("'%s'\n", p);
        //lineList[i].str = malloc(20 * sizeof(char));
        lineList[i].str = malloc(WORD_NB_MAX * sizeof(char));
        strcpy(lineList[i].str, p);
        p = strtok(NULL, " ");
        i++;
    }
    /*for (int i = 0; i < 2; i++){
        printf("lineList[%d].str : %s\n", i, lineList[i].str);
    }*/
    free(line2);
    return i;
}