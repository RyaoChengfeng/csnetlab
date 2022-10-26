#include "main.h"
#include "httpParse.h"

struct st_client {
    sockaddr_in st{};
    int fd{};
    std::string ip;
};

void worker(std::mutex *mutex,
            std::condition_variable *condition_variable,
            std::deque<st_client> *requests) {
    st_client client;
    while (true) {
        {
            std::unique_lock<std::mutex> lock(*mutex);
            while ((*requests).empty()) {
                (*condition_variable).wait(lock);
            }
            client = (*requests).front();
            (*requests).pop_front();
        }
        char buf[MAXSIZE];
        while (true) {
            memset(buf, 0, sizeof(buf));
            long bytes = read(client.fd, buf, sizeof(buf));
            buf[bytes] = '\0';
            if (bytes <= 0) {
                std::cout << "[disconnect] " << client.ip << ":" << ntohs(client.st.sin_port) << std::endl;
                close(client.fd);
                break;
            } else {
                printf("Receive %ld bytes from client:\n\n%s\n", bytes, buf);

                // 解析http
                unordered_map<string, string> mmap;
                // http://127.0.0.1:7888/index.html
                int code = http_parse(buf, mmap);

                string tmp = mmap["Host"];
                string mip = tmp.substr(0, tmp.find(":"));
                string mport = tmp.substr(tmp.find(":") + 1);
                cout << "IP地址：" << mip << endl;
                cout << "端口号：" << mport << endl;
                cout << "请求URL：" << mmap["url"] << endl;

                // 响应http
                http_response(client.fd, mmap);
            }
        }
    }
}


int main() {
    string host;
    int port;

    // 读取配置
    string conf_name = "conf.ini";
    string conf_addr = binpath + conf_name;
    FILE *fp = fopen(conf_addr.c_str(), "r");
    if (fp == nullptr) {
        printf("read conf.ini failed!");
        exit(0);
    }
    long file_size = get_file_size(conf_addr);
    char file_buff[file_size + 1];
    memset(file_buff, 0, file_size + 1);
    fread(file_buff, sizeof(char), file_size, fp);
    fclose(fp);
    unordered_map<string, string> conf;
    string line;
    string text = file_buff;
    while (true) {
        line = text.substr(0, text.find("\n"));
        text = text.substr(text.find("\n") + 1);
        conf.emplace(line.substr(0, line.find("=")), line.substr(line.find("=") + 1));
        if (text == "") {
            break;
        }
    }

    host = conf["host"];
    port = atoi(conf["port"].c_str());
    printf("server start listen %s:%d\n", host.c_str(), port);

    // server
    int serverfd, connfd;

    struct sockaddr_in st_serversock{};

    if ((serverfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket error: %s (errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    memset(&st_serversock, 0, sizeof(st_serversock));
    st_serversock.sin_family = AF_INET;
    st_serversock.sin_addr.s_addr = inet_addr(host.c_str());
    st_serversock.sin_port = htons(port);

    if (bind(serverfd, (struct sockaddr *) &st_serversock, sizeof(st_serversock)) < 0) {
        printf("bind error: %s (errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    if (listen(serverfd, 100) < 0) {   // 客户端最大连接数
        printf("listen error: %s (errno: %d)\n", strerror(errno), errno);
        exit(0);
    }

    // 处理client request
    printf("Waiting for request\n");

    std::thread threads[MAX_THREAD];
    std::condition_variable condition_variable;
    std::deque<st_client> requests;
    std::mutex mutex;
    for (auto &i: threads) {
        i = std::thread(worker, &mutex, &condition_variable, &requests);
    }

    while (true) {
        char clientIP[INET_ADDRSTRLEN] = "";
        struct sockaddr_in st_clientsock{};
        socklen_t client_len = sizeof(st_clientsock);
        if ((connfd = accept(serverfd, (struct sockaddr *) &st_clientsock, &client_len)) < 0) {
            printf("accept error: %s (errno: %d)\n", strerror(errno), errno);
            continue;
        } else {
            printf("client[%d] connected\n", connfd);
        }

        inet_ntop(AF_INET, &st_clientsock.sin_addr, clientIP, INET_ADDRSTRLEN);
        std::cout << "[Connect] " << clientIP << ":" << ntohs(st_clientsock.sin_port) << std::endl;
        // 线程处理
        {
            std::lock_guard<std::mutex> lock(mutex);
            requests.push_back(st_client{
                    .st=st_clientsock,
                    .fd=connfd,
                    .ip=clientIP
            });
            condition_variable.notify_one();
        }
    }
}

