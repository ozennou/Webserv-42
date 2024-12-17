#include "header.hpp"
#include <ResponseGenerator.hpp>
#include <Bond.hpp>

string CheckExec(string cgiExec) {
    if (access(cgiExec.c_str(), X_OK & F_OK) == -1)
        return "";
    char full_path[PATH_MAX];    
    if ((realpath(cgiExec.c_str(), full_path)) == NULL)
        return "";
    return string(full_path);
}

string getCgiExec(string cgiExt) {
    string res;
    if (cgiExt == ".py")
    {
        res = CheckExec("./cgi-exe/python-cgi");
        return res;
    }
    else if (cgiExt == ".php")
    {
        res = CheckExec("./cgi-exe/php-cgi");
        if(res == "")
            res = CheckExec("/usr/bin/php");
        return res;
    }
    else if (cgiExt == ".sh")
    {
        res = CheckExec("/bin/sh");
        return res;
    }
    else if (cgiExt == ".pl")
    {
        res = CheckExec("/usr/bin/perl");
        return res;
    }
    return res;
}

char** ResponseGenerator::cgiEnvs() {
    char **env = NULL;
    Uri& uri = bond->getUri();
    map<string, string> &headers = bond->getHeaders();
    map<string, string> envs;
    (void)uri;
    envs.insert(pair<string, string>("GATEWAY_INTERFACE", "CGI/1.1"));
    envs.insert(pair<string, string>("SERVER_NAME", *uri.getHostServer().getServerNames().begin()));
    envs.insert(pair<string, string>("SERVER_SOFTWARE", "WebServer-42/1.0.0"));
    envs.insert(pair<string, string>("SERVER_PROTOCOL", "HTTP/1.1"));
    envs.insert(pair<string, string>("SERVER_PORT", to_string(uri.port)));
    envs.insert(pair<string, string>("REQUEST_METHOD", (bond->getMethod() == GET) ? "GET" : "POST"));
    envs.insert(pair<string, string>("SCRIPT_NAME", uri.requestTarget));
    envs.insert(pair<string, string>("SCRIPT_FILENAME", uri.path));
    envs.insert(pair<string, string>("QUERY_STRING", uri.query));
    envs.insert(pair<string, string>("QUERY_STRING", uri.query));
    if (uri.cgiPath != "")
    {
        envs.insert(pair<string, string>("PATH_INFO", uri.cgiPath));
        envs.insert(pair<string, string>("PATH_TRANSLATED", uri.root + uri.cgiPath));
    }
    envs.insert(pair<string, string>("REMOTE_ADDR", bond->getRemoteAddr()));
    envs.insert(pair<string, string>("REMOTE_HOST", bond->getRemoteHost()));
    for (map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
    {
        map<string, string>::iterator check = envs.find(it->first);
        if (check == envs.end())
            envs.insert(pair<string, string>(it->first, it->second));
    }
    env = new(nothrow) char*[envs.size() + 1];
    if (!env)
        return NULL;
    size_t j = 0;
    for (map<string, string>::iterator it = envs.begin(); it != envs.end(); it++) {
        env[j] = new(nothrow) char[it->first.size() + it->second.size() + 2];
        if (!env[j]) {
            while (j > 0)
                delete[] env[--j];
            delete[] env;
            return NULL;
        }
        size_t k = 0;
        for (size_t m = 0; m < it->first.size(); m++)
            env[j][k++] = it->first[m];
        env[j][k++] = '=';
        for (size_t m = 0; m < it->second.size(); m++)
            env[j][k++] = it->second[m];
        env[j][k] = 0;
        j++;
    }
    env[j] = NULL;
    return env;
}

void delete_envs(char **envs, int *fd)
{
    if (envs)
    {
        for (int i = 0; envs[i]; i++)
            delete[] envs[i];
        delete[] envs;
    }
    if (fd)
    {
        close(fd[0]);
        close(fd[1]);
    }
}

void ResponseGenerator::CgiWait()
{
    int                 status;
    pair<int, pid_t>    infos = bond->getCgiInfos();
    if (waitpid(infos.second, &status, WNOHANG) != -1)
        return ;
    char            bf[1024];
    string          rspns;
    string          header;
    ssize_t          r;
    map<string, string> headers;
    while ((r = read(infos.first, bf, 1024)) > 0)
        rspns.append(bf, r);
    if (r == -1) {
        cout << "fail read: " << bond->isCgi << endl;
        bond->isCgi = false;
        bond->setCgiPhase(false);
        return ;
    }
    close(infos.first);
    while (rspns.find_first_of(CRLF) != string::npos)
    {
        header = rspns.substr(0, rspns.find_first_of(CRLF));
        rspns = rspns.substr(rspns.find_first_of(CRLF) + 2);
        if (header == "")
            break;
        headers.insert(std::pair<std::string, std::string>(
            header.substr(0, header.find_first_of(':')), 
            header.substr(header.find_first_of(':') + 1)
        ));

    }
    if (headers.find("Content-Length") == headers.end())
        headers.insert(std::pair<std::string, std::string>(
            "Content-Length", 
            " " + to_string(rspns.size())
        ));
    else
        headers["Content-Length"] = " " + to_string(rspns.size());
    // for (map<string, string>::iterator i = headers.begin(); i != headers.end(); i++)
    //     cout << RED << i->first  << "=" << i->second <<  RESET << endl;
        
    // cout << YELLOW << rspns << RESET << endl;
    // cout << WIFEXITED(status) << " " << WEXITSTATUS(status) <<  endl;
    if (WEXITSTATUS(status)) {
        // this->exception = new RequestParser::HttpRequestException("CGI Internal server error2", 500);
        // generateErrorMessage();
        return ;
    }
    generateCgiResponse(headers, rspns, bond->getUri());
    bond->setCgiPhase(true);
    bond->reset();
}

void ResponseGenerator::generateCgiResponse(map<string, string> &headers, string &reponse, Uri& uri) {
    stringstream ss;
    
    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);

    int statusCode = 200;
    ss << "HTTP/1.1 " << statusCode << statusCodeMap->find(statusCode)->second << CRLF;
    if (headers.find("Date") == headers.end())
        ss << "Date: " << date << CRLF;
    if (headers.find("Server") == headers.end())
        ss << "Server: " <<  *(uri.getHostServer().getServerNames().begin()) << CRLF;
    if (headers.find("Connection") == headers.end()) {
        if (bond->getConnectionState()) ss << "Connection: keep-alive" << CRLF;
        else ss << "Connection: close" << CRLF;
    }
    for (map<string, string>::iterator it = headers.begin(); it != headers.end(); it++)
        ss << it->first << ":" << it->second << CRLF;
    ss << CRLF;

    ss << reponse;

    int a = send(clientFd, ss.str().c_str(), ss.str().length(), 0);

    if (a == -1) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
    }
    bond->isCgi = false;
}