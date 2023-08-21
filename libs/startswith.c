#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "startswith.h"

int startswith(char strStartingWith[], char code[]) {
    int similarity = 0;
    int len = strlen(strStartingWith);
    for (int i = 0; i < len; i++){
        if (strStartingWith[i] == code[i]){
            similarity++;
        }
    }
    if (similarity >= len){
        return true;
    }
    return false;
}
