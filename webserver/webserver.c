#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>

#define PORT 8084
#define BUFFER_SIZE 3000

void error_webserver(const char s){
    fprintf(stdout, "(webserver) %s\n errno : %s\n", strerror(errno)); 
}

char* get_file_content(char* filename){ 
    FILE* f = fopen(filename, "r");
    long length = ftell(f);
    printf("length : %d\n");
    fclose(f);
    return "HTTP/1.0 200 OK\r\n hello world\r\n";
}

void webserver(char* folder){
    char listenbuff[BUFFER_SIZE];
    char* resp = "HTTP/1.0 200 OK\r\n"
"Server: webserver-c\r\n"
"Content-type: text/html\r\n\r\n"
"<html>hello, world</html>\r\n";

    struct sockaddr_in host_addr;
    int sockfd;
    int connectionfd;
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error_webserver("webserver (socket)");
        return 1;
    }
    printf("created socket\n");

    host_addr.sin_family = AF_INET;
    host_addr.sin_port = htons(PORT);
    host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(host_addr)) != 0) {
        error_webserver("webserver (bind)");
        return 1;
    }
    printf("socket successfully bound to address\n");
    if (listen(sockfd, SOMAXCONN) != 0) {
        error_webserver("webserver (listen)");
        return 1;
    }
    printf("server listening for connections\n");
    printf("waiting on port %d\n", PORT);
    for (;;){
        connectionfd = accept(sockfd, NULL, NULL);
        memset(listenbuff, 0, BUFFER_SIZE);
        read( connectionfd , listenbuff, BUFFER_SIZE);
        printf("buf : %s\n", listenbuff);
        //resp = get_file_content("out/index.html");
        write(connectionfd, resp, strlen(resp));
        close(connectionfd);
    }
}