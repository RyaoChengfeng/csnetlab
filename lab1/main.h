#include <cstdio>
#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cctype>
#include <unistd.h>
#include <sys/types.h>
#include <thread>
#include <cstdlib>
#include <cerrno>
#include <condition_variable>
#include <deque>
#include <arpa/inet.h>
#include <cstdlib>

#define MAX_THREAD 10
#define MAXSIZE 4096
//#define host "127.0.0.1"
//#define host INADDR_ANY
//#define port  7889
#define binpath "../lab1/"