#include "httpParse.h"

//    GET /index.html HTTP/1.1
//    Host: 127.0.0.1:7888
//    Connection: keep-alive
//            sec-ch-ua: "Google Chrome";v="105", "Not)A;Brand";v="8", "Chromium";v="105"
//    sec-ch-ua-mobile: ?0
//    sec-ch-ua-platform: "Linux"
//    Upgrade-Insecure-Requests: 1
//    User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/105.0.0.0 Safari/537.36
//    Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9
//    Sec-Fetch-Site: none
//    Sec-Fetch-Mode: navigate
//    Sec-Fetch-User: ?1
//    Sec-Fetch-Dest: document
//    Accept-Encoding: gzip, deflate, br
//    Accept-Language: zh-CN,zh;q=0.9,en-CN;q=0.8,en;q=0.7,zh-TW;q=0.6
//    Cookie: Hm_lvt_5998fcd123c220efc0936edf4f250504=1659365368


void parser_requestline(const string &text, unordered_map<string, string> &m_map) {
    string m_method = text.substr(0, text.find(" "));
    string m_url = text.substr(text.find_first_of(" ") + 1, text.find_last_of(" ") - text.find_first_of(" ") - 1);
    string m_protocol = text.substr(text.find_last_of(" ") + 1);
    m_map["method"] = m_method;
    m_map["url"] = m_url;
    m_map["protocol"] = m_protocol;
}

void parser_header(const string &text, unordered_map<string, string> &m_map) {
    if (!text.empty()) {
        if (text.find(": ") <= text.size()) {
            string m_type = text.substr(0, text.find(": "));
            string m_content = text.substr(text.find(": ") + 2);
            m_map[m_type] = m_content;
        } else if (text.find("=") <= text.size()) {
            string m_type = text.substr(0, text.find("="));
            string m_content = text.substr(text.find("=") + 1);
            m_map[m_type] = m_content;
        }
    }
}

void parser_param(const string &text, unordered_map<string, string> &m_map) {
    //username=ll&passwd=12345
    //cout << "post:   " << text << endl;
    string processd = "";
    string strleft = text;
    while (true) {
        processd = strleft.substr(0, strleft.find("&"));
        m_map[processd.substr(0, processd.find("="))] = processd.substr(processd.find("=") + 1);
        strleft = strleft.substr(strleft.find("&") + 1);
        if (strleft == processd)
            break;
    }
}


HTTP_CODE http_parse(char buff[], unordered_map<string, string> &m_map) {

    string m_head = "";
    string m_left = buff;
    int flag = 0;
    int do_post_flag = 0;
    int line = 0;
    while (true) {
        line++;
        m_head = m_left.substr(0, m_left.find("\r\n"));
        m_left = m_left.substr(m_left.find("\r\n") + 2);
        if (flag == 0) {
            flag = 1;
            cout << "line[" << line << "]: " << m_head << endl;
            parser_requestline(m_head, m_map);
        } else if (do_post_flag) {
            cout << "do: post" << endl;
            parser_param(m_head, m_map);
            break;
        } else {
            cout << "line[" << line << "]: " << m_head << endl;
            parser_header(m_head, m_map);
        }
        if (m_head == "") {
            do_post_flag = 1;
        }
        if (m_left == "") {
            break;
        }
    }

    if (m_map["method"] == "POST") {
        //cout << "request" << read_buff << endl;
        return POST_REQUEST;
    } else if (m_map["method"] == "GET") {
        return GET_REQUEST;
    } else {
        return NO_REQUEST;
    }
}

long get_file_size(string file_name) {    //得到文件大小
    FILE *fp = fopen(file_name.c_str(), "rb");
    if (fp == nullptr) {
        return -1;
    }
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);
    return size;
}

string get_file_type(string file_path) {    //返回响应报文的文件类型
    string tmp = file_path.substr(file_path.find(".") + 1);
    if (tmp.empty())
        return "text/plain";
    else if (tmp == "html" || tmp == "htm")
        return "text/html";
    else if (tmp == "css")
        return "text/css";
    else if (tmp == "gif")
        return "image/gif";
    else if (tmp == "jpeg" || tmp == "jpg")
        return "image/jpeg";
    else if (tmp == "png")
        return "image/png";
    else
        return "text/plain";
}

void http_response(int sock, unordered_map<string, string> &m_map) {
    if (m_map["method"] == "GET") {
        string file_name = m_map["url"].substr(m_map["url"].find("/") + 1);
        cout << "打开文件：" << file_name << endl;
        string file_path = binpath + file_name;
        FILE *fd = fopen(file_path.c_str(), "rb");
        if (fd == nullptr) {
            string rsp = HTTP404;
            send(sock, rsp.c_str(), strlen(rsp.c_str()), 0);
            return;
        }

        long file_size = get_file_size(file_path);
        if (file_size == -1){
            string rsp = HTTP404;
            send(sock, rsp.c_str(), strlen(rsp.c_str()), 0);
            return;
        }

//        char file_buff[file_size+1];
//        memset(file_buff, 0, file_size + 1);
//        fread(file_buff,sizeof(char),file_size,fd);
//        fclose(fd);        //关闭文件

        int opened_file = open(file_path.c_str(), O_RDONLY);
        void *memfile = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, opened_file, 0);
        //存储映射，防止因文件过大爆栈
        close(opened_file);

        string file_type = get_file_type(file_name);
        char buff[4096], FlTp[256], ConLen[256];
        memset(buff, 0, sizeof(buff));
        memset(ConLen, 0, sizeof(ConLen));
        memset(FlTp, 0, sizeof(FlTp));

        strcat(buff, "HTTP1.0 200 ok\r\n");        //响应报文的格式
        sprintf(FlTp, "Content-Type: %s charset=UTF-8\r\n", file_type.c_str());
        strcat(buff, FlTp);
        //	strcat(buff,"Content-Type: text/html;charset=UTF-8\r\n");
        sprintf(ConLen, "Content-Length: %ld\r\n\r\n", file_size);
        strcat(buff, ConLen);

        send(sock, buff, strlen(buff), 0);
//        send(sock, file_buff, file_size, 0);
        send(sock, memfile, file_size, 0);
        printf("Send successfully\n");
        printf("Send:\n%s\n", buff);

        printf("The file size is: %ld\n", file_size);
        printf("The context is \n");
//        write(1, memfile, file_size);
        printf("\n");

        munmap(memfile, file_size);
        return;
    } else {
        string rsp = HTTP400;
        send(sock, rsp.c_str(), strlen(rsp.c_str()), 0);
        return;
    }
}
