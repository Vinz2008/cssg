#include "syntax-highlighting.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser_config.h"

bool has_added_script_tag = false;
char* theme = NULL;

char* markdown_code_block_callback(const char* code, const int code_size, void* lang){
    char* script = "";
    char* lang_to_use = "js";
    if (lang != NULL){
        lang_to_use = (char*)lang;
    }
    if (!has_added_script_tag){
        script = "<script src=\"https://cdn.jsdelivr.net/npm/shiki\"></script>";
    }
    char* js_code_format = "<script> \
  shiki \
    .getHighlighter({ \
      theme: '%s', \
      langs: ['%s'], \
    }) \
    .then(highlighter => { \
      const code = highlighter.codeToHtml(`%s`, { lang: '%s' }); \
      var codes = document.getElementsByTagName('code'); \
      codes[codes.length-1].innerHTML = code \
    }) \
</script>";
    size_t js_code_length = strlen(js_code_format) + code_size + strlen(lang_to_use) * 2 + strlen(theme) + 1;
    char* js_code = malloc(js_code_length * sizeof(char));
    snprintf(js_code, js_code_length, js_code_format, theme, lang_to_use, code, lang_to_use);
    char* ret = malloc((strlen(script) + strlen(js_code)) * sizeof(char));
    memset(ret, 0, (strlen(script) + strlen(js_code)));
    //strcpy(ret, code);
    strcat(ret, script);
    strcat(ret, js_code);
    printf("ret : %s\n", ret);
    printf("code lang : %s\n", (char*)lang);
    free(js_code);
    return ret;
}

void init_syntax_highlighting(MMIOT* mmiot, config_t* config){
    mkd_e_code_format(mmiot, markdown_code_block_callback);
    theme = config->syntax_config.theme;
}