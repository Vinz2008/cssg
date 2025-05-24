#include "utils.h"
#include <string.h>

bool startswith(const char* s1, const char* s2){
    int len = strlen(s1);
    for (int i = 0; i < len; i++){
        if (s1[i] != s2[i]){
            return false;
        }
    }
    return true;
}
