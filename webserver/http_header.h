struct http_header {
    char* request_type; // GET or POST
    char* url;
};

struct http_header* parse_http_header(char* header);