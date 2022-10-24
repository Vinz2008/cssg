#include <mkdio.h>

void convert_markdown_to_html(char* md_path, char* html_path){
	FILE* in = fopen(md_path, "r");
	FILE* out = fopen(html_path, "w");
	MMIOT* mmiot = mkd_in(in, 0);
	markdown(mmiot, out, 0);
	fclose(in);
	fclose(out);
}
