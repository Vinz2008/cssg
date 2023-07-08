#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRING_FACTOR 3

int removeCharFromString(char charToRemove, char *str){
	int i,j;
	i = 0;
	while(i<strlen(str))
	{
    	if (str[i]==charToRemove)
    	{
        for (j=i; j<strlen(str); j++) {
            str[j]=str[j+1];
        }
    	}
    	else i++;
	}
	return 0;
}


struct string* createString(char* str){
    struct string* string = malloc(sizeof(struct string));
    string->pointer = strdup(str);
    memset(string->pointer, 0, strlen(str));
    string->length = strlen(str);
    string->allocated_size = strlen(str);
    return string;
}

void appendString(char c, struct string* string){
    if (string->length == string->allocated_size-1){
        string->allocated_size += STRING_FACTOR;
        string->pointer = realloc(string->pointer, string->allocated_size * sizeof(char));
        memset(string->pointer + string->length+1, 0, string->allocated_size-string->length+1);
    }
    string->pointer[string->length++] = c; 
}

struct string* reset_string(struct string* string){
    free(string->pointer);
    string->pointer = malloc(sizeof(char));
    string->length = 0;
    string->allocated_size = 0;
    return string;
}


void destroyString(struct string* string){
    free(string->pointer);
    free(string);
}