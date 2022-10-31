#include <iostream>
#include <cstdio>
#include <cstring>
#include <string>
#include <cstdlib>
#include <unordered_map>
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#include "main.h"

using std::string, std::unordered_map, std::cout, std::endl;

const string HTTP404 = "HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Length: 9\r\n\r\nNot Found";
const string HTTP400 = "HTTP/1.0 400 Bad Request\r\nConnection: close\r\nContent-Length: 11\r\n\r\nBad Request";

enum HTTP_CODE {
    NO_REQUEST,
    GET_REQUEST,
    POST_REQUEST,
    NO_RESOURCE,
    CLOSED_CONNECTION
};

void parser_requestline(const string &text, unordered_map<string, string> &mmap);

void parser_header(const string &text, unordered_map<string, string> &mmap);

void parser_param(const string &text, unordered_map<string, string> &mmap);

HTTP_CODE http_parse(char buff[], unordered_map<string, string> &mmap);

long get_file_size(string file_name);

string get_file_type(string file_path);

void http_response(int sock, unordered_map<string, string> &m_map);