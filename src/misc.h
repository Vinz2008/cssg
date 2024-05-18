#include "parser_config.h"

void clean(struct config_file* config);
void go_to_folder(const char* folder, const char* path, char* out);
void mkdir_if_not_exist(char* path);

struct runCommandReturn {
    char* stdout;
    int exit_status;
};

struct runCommandReturn run_command(const char* command, bool is_silent);

char* get_file_path_in_folder(const char* folder_path, const char* filename);