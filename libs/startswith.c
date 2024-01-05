#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "startswith.h"

int startswith(char* s1, char* s2) {
    int similarity = 0;
    int len = strlen(s1);
    for (int i = 0; i < len; i++){
        if (s1[i] == s2[i]){
            similarity++;
        }
    }
    return similarity >= len;
}
