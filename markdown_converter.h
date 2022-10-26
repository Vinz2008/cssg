#include "parser_config.h"

void convert_markdown_to_html(char* md_path, char* html_path);
void generate_html_files_recursive(char* article_folder, char* html_folder);
void insert_generated_html_in_default_template_recursive(char* temp_folder, char* html_folder, struct config_file* config);