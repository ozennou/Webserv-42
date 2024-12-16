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