#ifndef _WIN32

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <assert.h>
#include "../misc.h"
#include "http_header.h"
#include "../libs/utils_file.h"

#define PORT 8084
#define BUFFER_SIZE 5000

struct FileContent {
    bool is_binary;
    char* buffer;
    size_t size;
};

void error_webserver(const char* s){
    fprintf(stdout, "(webserver) %s\n errno : %s\n", s, strerror(errno));
    exit(1);
}


void open_browser_url(){
    char* url = malloc(sizeof(char) * 30);
    sprintf(url, "localhost:%d", PORT);
    char* cmd_start = "xdg-open ";
    char* cmd = malloc(sizeof(char) * (strlen(url) + strlen(cmd_start) + 1));
    sprintf(cmd, "%s%s", cmd_start, url);
    system(cmd);
}


char* get_file_extension(char* filename){
    bool found_period = false;
    char* temp = malloc(strlen(filename) * sizeof(char));
    memset(temp, 0, strlen(filename));
    for (int i = 0; i < strlen(filename); i++){
        if (filename[i] == '.'){
            found_period = true;
        }
        if (found_period){
            strncat(temp, filename + i, 1);
        }
    }
    return temp;
}

bool is_file_binary(char* file_extension){
    if (strcmp(file_extension, ".png") == 0 || strcmp(file_extension, ".jpg") == 0 || strcmp(file_extension, ".jpeg") == 0 || strcmp(file_extension, ".webp") == 0 || strcmp(file_extension, ".ico") == 0 || strcmp(file_extension, ".gif") == 0 || strcmp(file_extension, ".aac") == 0 || strcmp(file_extension, ".weba") == 0 || strcmp(file_extension, ".avi") == 0 || strcmp(file_extension, ".ts") == 0 || strcmp(file_extension, ".webm") == 0){
        return true;
    }
    if (strcmp(file_extension, ".html") == 0 || strcmp(file_extension, ".htm") == 0 || strcmp(file_extension, ".txt") == 0 || strcmp(file_extension, ".css") == 0 || strcmp(file_extension, ".js") == 0 || strcmp(file_extension, ".mjs") == 0 || strcmp(file_extension, ".json") == 0){
        return false;
    }
    return false;
}

char* get_general_filetype_from_file_extension(char* file_extension){
    if (strcmp(file_extension, ".png") == 0 || strcmp(file_extension, ".jpg") == 0 || strcmp(file_extension, ".jpeg") == 0 || strcmp(file_extension, ".webp") == 0 || strcmp(file_extension, ".ico") == 0 || strcmp(file_extension, ".gif") == 0){
        return "image";
    }
    if (strcmp(file_extension, ".html") == 0 || strcmp(file_extension, ".htm") == 0 || strcmp(file_extension, ".txt") == 0 || strcmp(file_extension, ".css") == 0 || strcmp(file_extension, ".js") == 0 || strcmp(file_extension, ".mjs") == 0 || strcmp(file_extension, ".json") == 0){
        return "text";
    }
    if (strcmp(file_extension, ".aac") == 0 || strcmp(file_extension, ".weba") == 0){
        return "audio";
    }
    if (strcmp(file_extension, ".avi") == 0 || strcmp(file_extension, ".ts") == 0 || strcmp(file_extension, ".webm") == 0){
        return "video";
    }
    return "text";
}

char* get_filetype_from_file_extension(char* file_extension){
    if (strcmp(file_extension, ".png") == 0){
        return "png";
    }
    if (strcmp(file_extension, ".jpg") == 0 || strcmp(file_extension, ".jpeg") == 0){
        return "jpg";
    }
    if (strcmp(file_extension, ".webp") == 0){
        return "webp";
    }
    if (strcmp(file_extension, ".ico") == 0){
        return "vnd.microsoft.icon";
    }
    if (strcmp(file_extension, ".txt") == 0){
        return "plain";
    }
    if (strcmp(file_extension, ".html") == 0 || strcmp(file_extension, ".htm") == 0){
        return "html";
    }
    if (strcmp(file_extension, ".css") == 0){
        return "css";
    }
    if (strcmp(file_extension, ".js") == 0 || strcmp(file_extension, ".mjs") == 0){
        return "javascript";
    }
    if (strcmp(file_extension, ".json") == 0){
        return "json";
    }
    if (strcmp(file_extension, ".aac") == 0){
        return "aac";
    }
    if (strcmp(file_extension, ".avi") == 0){
        return "x-msvideo";
    }
    if (strcmp(file_extension, ".gif") == 0){
        return "gif";
    }
    if (strcmp(file_extension, ".ts") == 0){
        return "mp2t";
    }
    if (strcmp(file_extension, ".webm") == 0){
        return "webm";
    }
    if (strcmp(file_extension, ".weba") == 0){
        return "webm";
    }
    if (strcmp(file_extension, ".bin") == 0){
        return "octet-stream";
    }

    return "html";
}

struct FileContent* get_file_content(char* filename){
    char* buffer;
    char* file_extension = get_file_extension(filename);
    bool is_file_binary_bool = is_file_binary(file_extension);
    printf("file extension : %s\n", file_extension);
    char* mode = "r";
    if (is_file_binary(file_extension)){
        mode = "rb";
    }
    FILE* f = fopen(filename, mode);
    if (f == NULL || fseek(f, 0, SEEK_END)) {
        error_webserver("file doesn't exist\n");
        return NULL;
    }
    long length = ftell(f);
    rewind(f);
    printf("length : %ld\n", length);
    char* general_file_type = get_general_filetype_from_file_extension(file_extension);
    char* file_type = get_filetype_from_file_extension(file_extension);
    char* http_header_not_formatted = "HTTP/1.0 200 OK\nServer: webserver-c\nContent-type: %s/%s\n\n";
    //char* http_header = "HTTP/1.0 200 OK\nServer: webserver-c\nContent-type: text/html\n\n";
    char* http_header = malloc((strlen(http_header_not_formatted) + strlen(file_type)) * sizeof(char));
    if (!http_header){
        error_webserver("error webserver : malloc\n");
    }
    sprintf(http_header, http_header_not_formatted, general_file_type, file_type);
    buffer = malloc(sizeof(char) * (length + strlen(http_header) + 1));
    strcpy(buffer, http_header);
    if (is_file_binary_bool){
        int size_read = fread(buffer + strlen(http_header), length + 1, 1, f);
        if (ferror(f)){
            perror("error when reading binary file");
        }
        printf("size read from binary file : %d\n", size_read);
        //buffer[length] = '\0';
    } else {
    int n = strlen(http_header);
    char c;
    while ((c = fgetc(f)) != EOF)
    {
        buffer[n++] = (char) c;
    }
    buffer[n] = '\0';
    }
    fclose(f);
    free(file_extension);
    struct FileContent* fileContent = malloc(sizeof(struct FileContent));
    fileContent->buffer = buffer;
    fileContent->is_binary = is_file_binary_bool;
    fileContent->size = length;
    return fileContent;
    //return buffer;
    //return "HTTP/1.0 200 OK\r\n hello world\r\n";
}


char* get_url_http_header(char* header){
    int pos = 0;
    char* url = malloc(200*sizeof(char));
    while (pos < strlen(header) && header[pos] != '/'){
        pos++;
    }
    pos++;
    if (pos == strlen(header) - 1){
        return NULL;
    }
    int url_pos = 0;
    /*url[url_pos] = '/'; 
    url_pos++;*/
    while (pos < strlen(header) && header[pos] != ' '){
        //printf("adding char : %c\n", header[pos]);
        url[url_pos] = header[pos];
        url_pos++;
        pos++;
    }
    url[url_pos] = '\0';
    //printf("url : %s\n", url);
    url = realloc(url, (url_pos+1)*sizeof(char));
    return url;
}

int webserver(char* folder){
    char* startFile = malloc(sizeof(char) * (strlen(folder) + strlen("index.html")));
    go_to_folder("index.html", folder, startFile);
    printf("startfile : %s\n", startFile);
    char listenbuff[BUFFER_SIZE];

    struct sockaddr_in host_addr;
    int sockfd;
    int connectionfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error_webserver("error creating socket)");
        return 1;
    }
    printf("created socket\n");

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int yes = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)); // option to make the web server use the port even if another process uses it. It is useful if you restart it a lot
    if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr)) != 0) {
        error_webserver("error binding socket");
        return 1;
    }
    printf("socket successfully bound to address\n");
    if (listen(sockfd, SOMAXCONN) != 0) {
        error_webserver("error listening to socket");
        return 1;
    }
    printf("server listening for connections\n");
    printf("waiting on port %d\n", PORT);
    open_browser_url();
    for (;;){
        connectionfd = accept(sockfd, NULL, NULL);
        memset(listenbuff, 0, BUFFER_SIZE);
        read( connectionfd , listenbuff, BUFFER_SIZE);
        printf("buf : %s\n", listenbuff);
        char* url = get_url_http_header(listenbuff);
        struct http_header* parsed_header = parse_http_header(listenbuff);
        url = parsed_header->url;
        printf("parsed header : url %s and request type %s\n", parsed_header->url, parsed_header->request_type);
        printf("url : %s\n", url);
        printf("strlen url : %ld\n", strlen(url));
        char* path = malloc(sizeof(char) * 1000);
        memset(path, 0, 1000);
        if (strlen(url) != 0){
            go_to_folder(url, folder, path); 
        } else {
            path = startFile;
        }
        if (!if_file_exists(path)){
            char* error404 = "HTTP/1.0 404 Not Found\r\nContent-Length: 16\r\nConnection: keep-alive\r\n\r\n<a>ERROR 404</a>\r\n\r\n";
            write(connectionfd, error404, strlen(error404));
        } else {
        if (strcmp(url, "favicon.ico") != 0){
        printf("path : %s\n", path);
        struct FileContent* fileContent = get_file_content(path);
        char* buffer = fileContent->buffer;
        size_t size = fileContent->size;
        printf("got file content of %ld length\n", size);
        char* temp_buffer_ptr = buffer;
        int length_to_send = strlen(buffer);
        if (fileContent->is_binary){
            length_to_send = fileContent->size;
        }
        //resp = get_file_content("out/index.html");
        ssize_t size_sent = write(connectionfd, temp_buffer_ptr, length_to_send);
        printf("sent %ld bytes\n", size_sent);
        free(buffer);
        }
        }
        close(connectionfd);
    }
    close(sockfd);
    return 0;
}

#endif