//#include <termios.h>
//#include <sys/ioctl.h>
//#include <sys/select.h>
#include <stdbool.h>
#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <unistd.h>
#include "webserver.h"

/*#ifndef _WIN32
int kbhit(){
    printf("kbhit check to press\n");
    static const int STDIN = 0;
    static bool initialized = false;
    if (!initialized) {
        // Use termios to turn off line buffering
        struct termios term;
        tcgetattr(STDIN, &term);
        term.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &term);
        setbuf(stdin, NULL);
        initialized = true;
    }

    int bytesWaiting;
    ioctl(STDIN, FIONREAD, &bytesWaiting);
    return bytesWaiting;
}
#endif*/

void* keyboard_watcher(void* arg){
    printf("keyboard_watcher\n");
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
    while(1){

        struct pollfd fds[1];
        fds[0].fd = STDIN_FILENO;
        fds[0].events = POLLIN;

        // Check if there is data available to be read
        int result = poll(fds, 1, 0);

        if (result == -1) {
            perror("poll");
            exit(EXIT_FAILURE);
        } else if (result > 0) {
            // Data available to be read
            char buffer[256];
            ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer));

            if (bytesRead > 0) {
                // Process the input
                //printf("Read %zd bytes: %.*s\n", bytesRead, (int)bytesRead, buffer);
            } else if (bytesRead == 0) {
                // EOF reached
                printf("EOF reached\n");
                webserver_memory_cleanup();
                exit(0);
            } else {
                perror("read");
                exit(EXIT_FAILURE);
            }
        }

    /*if(kbhit()){
        int key_pressed = getchar();
        printf("key pressed : %c\n", key_pressed);
        if (key_pressed == EOF){
            webserver_memory_cleanup();
            exit(0);
        }
    }*/
    }
    //return NULL;
}

void create_keyboard_watcher(){
    pthread_t thr;
    pthread_create(&thr, NULL, keyboard_watcher, NULL);
    //pthread_detach(thr);
}