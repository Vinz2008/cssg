#include "parser_config.h"

void clean(struct config_file* config);
void go_to_folder(char* folder, char* path, char* out);
void mkdir_if_not_exist(char* path);
int run_command(char* command);