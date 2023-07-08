#include <stddef.h>

int removeCharFromString(char charToRemove, char *str);

struct string {
    char* pointer;
    size_t allocated_size;
    size_t length;
};

struct string* createString(char* str);
void appendString(char c, struct string* string);
struct string* reset_string(struct string* string);
void destroyString(struct string* string);
