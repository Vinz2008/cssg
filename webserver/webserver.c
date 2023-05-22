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

#define PORT 8084
#define BUFFER_SIZE 5000

void error_webserver(const char* s){
    fprintf(stdout, "(webserver) %s\n errno : %s\n", s, strerror(errno));
    exit(1);
}


void open_browser_url(){
    char* url = malloc(sizeof(char) * 300);
    sprintf(url, "localhost:%d", PORT);
    char* cmd_start = "xdg-open ";
    char* cmd = malloc(sizeof(char) * (strlen(url) + strlen(cmd_start) + 1));
    sprintf(cmd, "%s%s", cmd_start, url);
    system(cmd);
}

char* get_file_extension(char* filename){
    bool found_period = false;
    char* temp = malloc(strlen(filename));
    memset(temp, 0, sizeof(temp));
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

char* get_file_content(char* filename){
    char* buffer;
    FILE* f = fopen(filename, "r");
    if (f == NULL || fseek(f, 0, SEEK_END)) {
        error_webserver("file doesn't exist\n");
        return NULL;
    }
    long length = ftell(f);
    rewind(f);
    printf("length : %d\n");
    //char* file_extension = get_file_content(file_extension);
    //printf("file extension : %s\n", file_extension);
    char* file_type = "html";
    char* http_header_not_formatted = "HTTP/1.0 200 OK\nServer: webserver-c\nContent-type: text/%s\n\n";
    //char* http_header = "HTTP/1.0 200 OK\nServer: webserver-c\nContent-type: text/html\n\n";
    char* http_header = malloc((strlen(http_header_not_formatted) + strlen(file_type)) * sizeof(char));
    sprintf(http_header, http_header_not_formatted, file_type);
    buffer = malloc(sizeof(char) * (length + strlen(http_header) + 1));
    strcpy(buffer, http_header);
    int n = strlen(http_header);
    char c;
    while ((c = fgetc(f)) != EOF)
    {
        buffer[n++] = (char) c;
    }
    buffer[n] = '\0';
    fclose(f);
    return buffer;
    //return "HTTP/1.0 200 OK\r\n hello world\r\n";
}


char* get_url_http_header(char* header){
    int pos = 0;
    char* url = malloc(50*sizeof(char));
    while (pos < strlen(header) && header[pos] != '/'){
        pos++;
    }
    pos++;
    if (pos == strlen(header) - 1){
        return NULL;
    }
    int slash_pos = pos;
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
    char* resp = "HTTP/1.0 200 OK\r\n"
"Server: webserver-c\r\n"
"Content-type: text/html\r\n\r\n"
"<html>hello, world</html>\r\n";

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
        printf("url : %s\n", url);
        printf("strlen url : %ld\n", strlen(url));
        char* path = malloc(sizeof(char) * 1000);
        memset(path, 0, 1000);
        if (strlen(url) != 0){
            go_to_folder(url, folder, path); 
        } else {
            path = startFile;
        } 
        if (strcmp(url, "favicon.ico") != 0){
        char* buffer = get_file_content(path);
        printf("got file content\n");
        //resp = get_file_content("out/index.html");
        write(connectionfd, buffer, strlen(buffer));
        }
        close(connectionfd);
    }
    close(sockfd);
    return 0;
}