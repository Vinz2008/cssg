int webserver(char* folder);


#ifdef _WIN32
#define SOCKET_TYPE unsigned int
#else
#define SOCKET_TYPE int
#endif


char* get_file_extension(char* filename);