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
    int                 status = 0;
    pid_t               pid;
    pair<int, pid_t>    infos = bond->getCgiInfos();
    pid = waitpid(infos.second, &status, WNOHANG);
    if (pid <= 0)
        return;
    if (WIFSIGNALED(status) && WTERMSIG(status) == SIGALRM) {
        close(infos.first);
        this->exception = new RequestParser::HttpRequestException("CGI Timeout", 502);
        generateErrorMessage();
        return ;
    }
    if (WEXITSTATUS(status)) {
        close(infos.first);
        this->exception = new RequestParser::HttpRequestException("CGI Internal server error", 500);
        generateErrorMessage();
        return ;
    }
    char            bf[1024];
    string          rspns;
    string          header;
    map<string, string> headers;
    ssize_t          r;
    while ((r = read(infos.first, bf, 1024)) > 0)
        rspns.append(bf, r);
    close(infos.first);
    if (r == -1) {
        bond->isCgi = false;
        bond->setCgiPhase(false);
        return ;
    }
    if (rspns.find("\r\n\r\n") != string::npos)
    {
        while (rspns.find(CRLF) != string::npos)
        {
            header = rspns.substr(0, rspns.find(CRLF));
            rspns = rspns.substr(rspns.find(CRLF) + 2);
            if (header == "")
                break;
            headers.insert(std::pair<std::string, std::string>(
                header.substr(0, header.find(':')), 
                header.substr(header.find(':') + 1)
            ));
        }
    }
    
    if (headers.find("Content-Length") == headers.end())
        headers.insert(std::pair<std::string, std::string>(
            "Content-Length", 
            " " + to_string(rspns.size())
        ));
    else
        headers["Content-Length"] = " " + to_string(rspns.size());
    generateCgiResponse(headers, rspns, bond->getUri());
    bond->isCgi = false;
    bond->setCgiPhase(true);
    bond->reset();
}

void ResponseGenerator::CGI() {
    bool isPost = (bond->getMethod() == POST);
    Uri& uri = bond->getUri();
    int postFd = -1;
    if (isPost)
    {
        postFd = open(bond->getUploader().file.c_str(), O_RDONLY, 0644);
        if (unlink(bond->getUploader().file.c_str()) == -1 || postFd == -1)
        {
            close(postFd);
            this->exception = new RequestParser::HttpRequestException("Cgi executable not found or don't have the right permessions", 500);
            generateErrorMessage();
            return ;
        }
    }
    string cgiExec = getCgiExec(uri.getCgiExt());
    if (cgiExec == "")
    {
        close(postFd);
        this->exception = new RequestParser::HttpRequestException("Cgi executable not found or don't have the right permessions", 500);
        generateErrorMessage();
        return ;
    }
    char **envs = cgiEnvs();
    
    if (envs == NULL)
    {
        close(postFd);
        delete_envs(envs, NULL);
        this->exception = new RequestParser::HttpRequestException("envs allocations fails", 500);
        generateErrorMessage();
        return ;
    }
    int fd[2];
    if (pipe(fd) == -1)
    {
        close(postFd);
        delete_envs(envs, NULL);
        this->exception = new RequestParser::HttpRequestException("pipe fails", 500);
        generateErrorMessage();
        return ;
    }
    pid_t p = fork();
    if (p == -1)
    {
        close(postFd);
        delete_envs(envs, fd);
        this->exception = new RequestParser::HttpRequestException("fork fails", 500);
        generateErrorMessage();
        return ;
    }
    if (!p)
    {
        alarm(bond->getCgiTimeout());
        close(fd[0]);
        if (dup2(fd[1], STDOUT_FILENO) == -1)
        {
            delete_envs(envs, fd);
            exit(52);
        }
        close(fd[1]);
        if (isPost)
        {
            if (dup2(postFd, STDIN_FILENO) == -1)
            {
                delete_envs(envs, fd);
                exit(52);
            }
            close(postFd);
        }
        if (chdir(uri.root.c_str()) == -1) {
            delete_envs(envs, NULL);
            exit(52);
        }
        const char *av[6];
        av[0] = cgiExec.c_str();
        if (uri.getCgiExt() == ".php") {
            av[1] = "-d";
            av[2] = "cgi.force_redirect=0";
            av[3] = "-f";
            av[4] = uri.path.c_str();
            av[5] = NULL;
        }
        else {
            av[1] = uri.path.c_str();
            av[2] = NULL;
        }
        if (execve(cgiExec.c_str(), const_cast<char**>(av), envs) == -1)
        {
            delete_envs(envs, NULL);
            exit(52);
        }
    }
    delete_envs(envs, NULL);
    close(fd[1]);
    close(postFd);
    bond->setCgiInfos(fd[0], p);
    cout << fd[0] << endl;
    bond->isCgi = true;
    CgiWait();
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