#ifndef _WIN32

#include "webserver.h"

#include "../src/config.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <string.h>

#ifdef _WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#include "windows.h"
#include "../src/windows.h"

#ifdef __APPLE__
#ifndef st_mtime
#define st_mtime st_mtimespec.tv_sec
#endif
#endif


//#include <sys/mman.h>
//#include <fcntl.h>
#include <stdlib.h>
//#include <assert.h>
#include "../src/misc.h"
#include "http_header.h"
#include "file_watcher.h"
#include "../src/utils_file.h"
#include "../src/utils.h"
#include "keyboard_watcher.h"

#define PORT 8084
#define BUFFER_SIZE 10000

#include <magic.h>

struct FileContent {
    bool is_binary;
    char* buffer;
    size_t size;
};

char* startFile = NULL;

void error_webserver(const char* s){
    fprintf(stderr, "(webserver) %s\n errno : %s\n", s, strerror(errno));
    exit(1);
}

void error_file_detection(const char* s){
    fprintf(stderr, "(webserver) error in file detection : %s\n", s);
    exit(1);
}

void webserver_memory_cleanup(){
    free(startFile);
}

void open_browser_url(){
    char* url = malloc(sizeof(char) * 100);
    sprintf(url, "localhost:%d", PORT);
    char* cmd_start = "xdg-open ";
    char* cmd = malloc(sizeof(char) * (strlen(url) + strlen(cmd_start) + 1));
    sprintf(cmd, "%s%s", cmd_start, url);
    run_command(cmd, IS_SILENT);
    free(cmd);
    free(url);
}

// get the file extension without the dot
char* get_file_extension(char* filename){
    bool found_period = false;
    char* temp = malloc(strlen(filename) * sizeof(char));
    memset(temp, 0, strlen(filename));
    int period_pos = -1;
    for (int i = strlen(filename)-1; i > 0; i--){
        if (filename[i] == '.'){
            found_period = true;
            period_pos = i;
        }
    }
    if (found_period){
        //printf("%s + %d : %s\n", filename, period_pos, filename + period_pos + 1);
        strcpy(temp, filename + period_pos + 1);
    }
    return temp;
}

char* get_filetype_from_filename(char* filename){
    magic_t m = magic_open(MAGIC_MIME_TYPE);
    if (magic_errno(m) > 0){
        error_file_detection(magic_error(m));
    }
    magic_load(m, NULL);
    if (magic_errno(m) > 0){
        error_file_detection(magic_error(m));
    }
    char* filetype = (char*)magic_file(m, filename);
    if (magic_errno(m) > 0){
        error_file_detection(magic_error(m));
    }
    printf("filetype : %s\n", filetype);
    filetype = strdup(filetype);
    //magic_close(m);
    return filetype;
}

bool is_file_binary(char* filename){
    char* filetype = get_filetype_from_filename(filename);
    printf("ISFILEBIN filetype: %s, startswith text/ bool : %d\n", filetype,startswith("text/", (char*)filetype));
    bool ret = !startswith("text/", (char*)filetype);
    free(filetype);
    return ret;
}

struct FileContent* get_file_content(char* filename){
    char* buffer;
    char* file_extension = get_file_extension(filename);
    bool is_file_binary_bool = is_file_binary(filename);
    printf("file extension : %s\n", file_extension);
    char* mode = "r";
    if (is_file_binary(filename)){
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
    char* http_header_not_formatted = "HTTP/1.0 200 OK\nServer: webserver-c\nContent-type: %s\n\n";
    char* file_type = get_filetype_from_filename(filename);
    printf("mime libmagic : %s\n", file_type);
    char* http_header = malloc((strlen(http_header_not_formatted) + strlen(file_type)) * sizeof(char));
    if (!http_header){
        error_webserver("malloc\n");
    }
    sprintf(http_header, http_header_not_formatted, file_type);
    free(file_type);
    buffer = malloc(sizeof(char) * (length + strlen(http_header) + 1));
    strcpy(buffer, http_header);
    if (is_file_binary_bool){
        //length = length + length/2;
        int size_read = fread(buffer /*+ strlen(http_header)*/, length + 1, 1, f);
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
    free(http_header);
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
        free(url);
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


struct http_header* parsed_header = NULL;

void atexit_webserver(){
    if (parsed_header){
    destroy_http_header(parsed_header);
    }
}

int webserver(char* folder){
    create_file_watcher(folder);
    create_keyboard_watcher();
    atexit(atexit_webserver);
    startFile = malloc(sizeof(char) * (strlen(folder) + strlen("index.html")+2));
    go_to_folder("index.html", folder, startFile);
    printf("startfile : %s\n", startFile);
    char listenbuff[BUFFER_SIZE];

#ifdef _WIN32
    WIN32InitSocket();
#endif

    struct sockaddr_in host_addr;
    SOCKET_TYPE sockfd;
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
        read(connectionfd , listenbuff, BUFFER_SIZE);
        printf("buf : %s\n", listenbuff);
        //char* url = get_url_http_header(listenbuff); // old implementation
        parsed_header = parse_http_header(listenbuff);
        char* url = parsed_header->url;
        printf("parsed header : url %s and request type \n", parsed_header->url/*, parsed_header->request_type*/); // TODO : find why the request type is bugged when it is this pic
        printf("url : %s\n", url);
        printf("strlen url : %ld\n", strlen(url));
        char* path = malloc(sizeof(char) * 1000);
        memset(path, 0, 1000);
        if (strlen(url) != 0){
            go_to_folder(url, folder, path); 
        } else {
            path = startFile;
        }
        printf("path file : %s\n", path);
        if (!if_file_exists(path)){
            char* error404 = "HTTP/1.0 404 Not Found\r\nContent-Length: 16\r\nConnection: keep-alive\r\n\r\n<a>ERROR 404</a>\r\n\r\n";
            write(connectionfd, error404, strlen(error404));
        } else {
        if (strcmp(url, "favicon.ico") != 0){
        printf("path : %s\n", path);
        struct FileContent* fileContent = get_file_content(path);
        //free(path);
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
        free(fileContent->buffer);
        free(fileContent);
        }
        }
        destroy_http_header(parsed_header);
        parsed_header = NULL;
        close(connectionfd);
    }

    webserver_memory_cleanup();
    
#ifdef _WIN32
    WIN32QuitSocket();
    WIN32CloseSocket();
#else
    shutdown(sockfd, SHUT_RDWR);
    close(sockfd);
#endif
    return 0;
}

#endif