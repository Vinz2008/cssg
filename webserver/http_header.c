#include "http_header.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include "../libs/string.h"

struct parsingContext {
    int pos;
    struct string* IdentifierStr;
    struct string* Url;
};

enum Token {
    tok_identifier = -1,
    tok_number = -2,
    tok_eof = -3,
    tok_url = -4,
};

void parser_error(char* format, ...){
    va_list args;
    printf("ERROR http header parsing : ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
    exit(1);
}

int getToken(char* header, struct parsingContext* context){
    if (context->pos+1 >= strlen(header)){
        return tok_eof;
    }
    printf("context->pos : %d\n", context->pos);
    while (isspace(header[context->pos])){
        context->pos++;
    }
    if (header[context->pos] == '/'){
        context->Url = reset_string(context->Url);
        context->pos++;
        //appendString(header[context->pos], context->Url);
        while (isalpha(header[context->pos]) || header[context->pos] == '/' || isalnum(header[context->pos]) || header[context->pos] == '.'){
            appendString(header[context->pos], context->Url);
            context->pos++;
        }
        return tok_url;
    }
    if (isalnum(header[context->pos])){
        printf("first eader[context->pos] : %c\n", header[context->pos]);
        context->IdentifierStr = reset_string(context->IdentifierStr);
        while (isalpha(header[context->pos])){   
            appendString(header[context->pos], context->IdentifierStr);
            context->pos++;
        }
        return tok_identifier;
    }

    return header[context->pos];
}

struct http_header* parse_http_header(char* header){
    printf("strlen header : %ld\n", strlen(header));
    struct parsingContext* context = malloc(sizeof(struct parsingContext));
    context->pos = 0;
    context->IdentifierStr = createString("");
    context->Url = createString("");
    struct http_header* parsed_header = malloc(sizeof(struct http_header));
    int CurTok;
    CurTok = getToken(header, context);
    printf("CurTok : %d\n", CurTok);
    if (CurTok != tok_identifier){
        parser_error("expected identifier for request type");
    }
    printf("found request type : %s\n", context->IdentifierStr->pointer);
    appendString('\0', context->IdentifierStr);
    parsed_header->request_type = strdup(context->IdentifierStr->pointer);
    CurTok = getToken(header, context);
    if (CurTok != tok_url){
        parser_error("expected url after request type");
    }
    appendString('\0', context->Url);
    parsed_header->url = strdup(context->Url->pointer);
    printf("parsed_header url length %ld\n", strlen(parsed_header->url));
    destroyString(context->IdentifierStr);
    destroyString(context->Url);
    free(context);
    return parsed_header;
}