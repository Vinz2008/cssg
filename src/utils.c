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


bool is_whitespace(char c){
    return c == ' ' || c == '\t';
}

void trim(char* s){
    int start_pos = 0;
    while (is_whitespace(s[start_pos])){
        start_pos++;
    }

    size_t s_len = strlen(s);

    int end_pos = s_len-1;
    while (is_whitespace(s[end_pos]) || s[end_pos] == '\n'){
        end_pos--;
    }
    
    for (int pos = start_pos; pos < end_pos; pos++){
        s[pos-start_pos] = s[pos];
    }

    memset(s + end_pos + 1, 0, s_len-end_pos-1);
}

