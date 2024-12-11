/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 20:52:22 by mlouazir          #+#    #+#             */
/*   Updated: 2024/12/10 17:53:47 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseGenerator.hpp"
#include "../include/Bond.hpp"

ResponseGenerator::ResponseGenerator( ) {
}

ResponseGenerator::ResponseGenerator( const ResponseGenerator& obj ) {
    *this = obj;
}

ResponseGenerator& ResponseGenerator::operator=( const ResponseGenerator& obj ) {
    if (this != &obj) {
        this->clientFd = obj.clientFd;
        this->toRead = obj.toRead;
        this->exception = obj.exception;
        this->bond = obj.bond;
        this->statusCodeMap = obj.statusCodeMap;
        this->errorPages = obj.errorPages;
        this->isRedirect = false;
    }
    return *this;
}

ResponseGenerator::ResponseGenerator( int clientFd, map<int, string>& statusCodeMap ) : clientFd(clientFd), exception(NULL), statusCodeMap(&statusCodeMap) {
    toRead = 0;
}

ResponseGenerator::~ResponseGenerator( ) {
    if (exception) delete exception;
}

void ResponseGenerator::reset( ) {
    toRead = -1;
    isRedirect = false;
    ifs.close(); ifs.clear();
}

void ResponseGenerator::setException( RequestParser::HttpRequestException* exc ) {
    if (!exception) this->exception = new RequestParser::HttpRequestException(*exc);
}

void ResponseGenerator::setBondObject( Bond* bondd ) {
    this->bond = bondd;
}

void ResponseGenerator::setRedirect( pair<int, string> info ) {
    isRedirect = true;
    redirectPair = info;
}

void ResponseGenerator::setErrorPages( map<int, string> errorPagess ) {
    this->errorPages = errorPagess;
}

void ResponseGenerator::generateErrorMessage( ) {
    stringstream ss;
    Uri uri = bond->getUri();

    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);

    logging(exception->message, WARNING, NULL, 0);
    ss << "HTTP/1.1 " << exception->statusCode << " " << statusCodeMap->find(exception->statusCode)->second << CRLF;
    ss << "Date: " << date << CRLF;
    ss << "Server: " <<  *(uri.getHostServer().getServerNames().begin()) << CRLF;
    ss << "Connection: close" << CRLF;
    
    if (exception->statusCode == 416) ss << "Content-Range: */" << uri.getResourceSize() << CRLF;


    // Adding Error Pages
    if (errorPages.find(exception->statusCode) != errorPages.end()) {
        ifstream    in;
        string      str;
        string      buffer = "";
        size_t      length = 0;

        in.open(errorPages.find(exception->statusCode)->second);
        if (!in.fail()) {
            while (!in.eof()) {
                getline(in, str);
                length += str.length();
                buffer += str;
            }

            ss << "Content-Length: " << length << CRLF;
            ss << CRLF;
            ss << buffer;
        } else {
            ss << "Content-Length: 0" << CRLF;
            ss << CRLF;
        }
    } else {
        ss << "Content-Length: 0" << CRLF;
        ss << CRLF;
    }

    send(clientFd, ss.str().c_str(), ss.str().length(), 0);

    delete exception; exception = NULL;

    bond->reset();
}

void ResponseGenerator::completeRangeMessage( ) {
    string fileBuffer;
    size_t size = (toRead / 1620) >= 1 ? 1620 : toRead;
    char buf[size + 1];

    ifs.read(buf, size);
    if (ifs.bad()) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
        return ;
    }

    toRead -= ifs.gcount();
     
    fileBuffer.append(buf, ifs.gcount());

    int a = send(clientFd, fileBuffer.c_str(), fileBuffer.length(), 0);

    if (a == -1) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
        return ;
    }

    if (!toRead) {
        bond->reset();
        return ;
    }

}

void ResponseGenerator::completeNormalMessage( ) {
    string fileBuffer;
    char buf[1621];

    ifs.read(buf, 1620);
    if (ifs.bad()) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
        return ;
    }

    fileBuffer.append(buf, ifs.gcount());

    int a = send(clientFd, fileBuffer.c_str(), fileBuffer.length(), 0);

    if (a == -1) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
        return ;
    }

    if (ifs.eof()) {
        bond->reset();
        return ;
    }
}

void ResponseGenerator::generateValidMessage( int statusCode, Uri& uri, string& contentType, string& fileBuffer, size_t rangeFirst, size_t rangeLast ) {
    stringstream ss;
    
    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);
    
    struct stat result;

    // Normal HTTP Header
    ss << "HTTP/1.1 " << statusCode << statusCodeMap->find(statusCode)->second << CRLF;
    ss << "Date: " << date << CRLF;

    // Optional Case
    if (!stat(uri.path.c_str(), &result)) {
        struct tm datetime2 = *localtime(&result.st_mtime);
        char lastModified[40];
        strftime(lastModified, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime2);

        ss << "Last Modified: " << lastModified << CRLF;
    }
    
    // Normal HTTP Headers
    ss << "Server: " <<  *(uri.getHostServer().getServerNames().begin()) << CRLF;
    ss << "Content-Type: " << contentType << CRLF;
    ss << "Accept-Ranges: bytes" << CRLF;

    // In Case of Range request
    if (statusCode == 206) {
        ss << "Content-Range: bytes " << rangeFirst << "-" << rangeLast << "/" << uri.getResourceSize() << CRLF;
        if (bond->getRangeType() == INT_RANGE) ss << "Content-Length: " << (rangeLast - rangeFirst + 1) << CRLF;
        if (bond->getRangeType() == SUFFIX_RANGE) ss << "Content-Length: " << (rangeLast - (rangeLast - rangeFirst)) << CRLF;
    }
    else ss << "Content-Length: " << uri.getResourceSize() << CRLF;

    // Connection State
    if (bond->getConnectionState()) ss << "Connection: keep-alive" << CRLF;
    else ss << "Connection: close" << CRLF;
    
    ss << CRLF;

    // Adding The Paylod
    ss << fileBuffer;

    int a = send(clientFd, ss.str().c_str(), ss.str().length(), 0);

    if (a == -1) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
    }
}

void ResponseGenerator::NormalGETResponse( ) {
    if (bond->getResponseState() == OPEN) {
        completeNormalMessage();
        return ;
    }

    Uri uri = bond->getUri();
    string contentType = "application/octet-stream";

    if (uri.path.rfind('.') != string::npos) {
        string tmp = uri.path.substr(uri.path.rfind('.') + 1);

        map<string, string> mimeTypes = uri.getHostServer().getMimeTypes();

        map<string, string>::iterator it = mimeTypes.find(tmp);

        if (it != mimeTypes.end()) contentType = it->second;
    }

    ifs.open(uri.path.c_str(), ios::in | ios::binary);
    if (!ifs.is_open()) {
        this->exception = new RequestParser::HttpRequestException("System Error-The Opening Of The File", 500);
        generateErrorMessage();
        return ;
    }

    string fileBuffer;
    char buf[1621];
    
    ifs.read(buf, 1620);
    
    if (ifs.bad()) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
        return ;
    }

    fileBuffer.append(buf, ifs.gcount());

    generateValidMessage(200, uri, contentType, fileBuffer, -1, -1);

    if (ifs.eof()) {
        bond->reset();
    } else {
        bond->setResponseState(OPEN);
    }
}

void ResponseGenerator::RangeGETResponse( ) {
    if (bond->getResponseState() == OPEN) {
        completeRangeMessage();
        return ;
    }

    Uri uri = bond->getUri();
    string contentType = "application/octet-stream";

    if (uri.path.rfind('.') != string::npos) {
        string tmp = uri.path.substr(uri.path.rfind('.') + 1);

        map<string, string> mimeTypes = uri.getHostServer().getMimeTypes();

        map<string, string>::iterator it = mimeTypes.find(tmp);

        if (it != mimeTypes.end()) contentType = it->second;
    }
    
    ifs.open(uri.path.c_str(), ios::in | ios::binary);
    if (!ifs.is_open()) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
        return ;
    }
    
    size_t rangeFirst;
    size_t rangeLast;
    if (bond->getRangeType() == INT_RANGE) {
        rangeFirst = getRangeValue(bond->getRangeFirst());
        rangeLast = getRangeValue(bond->getRangeLast());
        
        if (!rangeLast || rangeLast >= uri.getResourceSize()) rangeLast = uri.getResourceSize() - 1;

        ifs.seekg(rangeFirst);

        toRead = rangeLast - rangeFirst + 1;
    } else {
        rangeFirst = getRangeValue(bond->getRangeLast());
        rangeLast = uri.getResourceSize() - 1;

        ifs.seekg(rangeLast - rangeFirst);

        toRead = rangeLast - (rangeLast - rangeFirst);
    }

    string fileBuffer;
    size_t size = (toRead / 1620) >= 1 ? 1620 : toRead;
    char buf[size + 1];
    
    ifs.read(buf, size);
    if (ifs.bad()) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
        return ;
    }

    toRead -= ifs.gcount();
     
    fileBuffer.append(buf, ifs.gcount());

    generateValidMessage(206, uri, contentType, fileBuffer, rangeFirst, rangeLast);

    if (!toRead) {
        bond->reset();
    } else {
        bond->setResponseState(OPEN);
    }
}

string ResponseGenerator::dirlisting()
{
    Uri     uri = bond->getUri();
    stringstream res;
    DIR* dir = opendir(uri.path.c_str());
    struct dirent *entry;
    struct stat result;

    if (!dir) {
        this->exception = new RequestParser::HttpRequestException("No permission to list the directory", 403);
        generateErrorMessage();
        return "";
    }

    res << "<!DOCTYPE HTML><html><head>";
    res << "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
    res << "<title>Index of " << uri.requestTarget << "</title>";
    res << "<style>";
    res << "body { font-family: Arial, sans-serif; max-width: 800px; margin: 0 auto; padding: 20px; }";
    res << ".file-list { font-weight: 300; list-style-type: none; padding: 0; margin-top: 20px; }";
    res << "li { display: flex; justify-content: space-between; margin: 5px 0; padding: 5px 0; border-bottom: 1px solid #eee; }";
    res << ".header { font-weight: bold; background-color: #f4f4f4; }";
    res << ".file-name { font-weight: bold; text-decoration: none; color: #007bff; flex: 2; }";
    res << ".file-details { display: flex; flex: 1; justify-content: space-between; }";
    res << ".file-type { font-style: italic; color: gray; text-align: right; margin-left: 10px; }";
    res << "</style></head><body>";

    res << "<h1>Index of " << uri.requestTarget << "</h1>";
    res << "<ul class='file-list'>";
    res << "<li class='header'>";
    res << "<span class='file-name'>File name</span>";
    res << "<div class='file-details'>";
    res << "<span>Last modified</span>";
    res << "<span>Size</span>";
    res << "<span>File type</span>";
    res << "</div></li>";
    while ((entry = readdir(dir)) != nullptr) {
        string full_path = uri.path + "/" + string(entry->d_name);

        if (entry->d_name[0] == '.' && entry->d_name[1] == '\0')
            continue ;
        res << "<li><a href='"<< uri.requestTarget << (uri.requestTarget[uri.requestTarget.length() - 1] == '/'? "": "/")<< entry->d_name << "'>" << entry->d_name << "</a>";
        if (!stat(full_path.c_str(), &result)) {
            struct tm datetime2 = *localtime(&result.st_mtime);
            char lastModified[40];
            strftime(lastModified, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime2);
            res << "<a>" << lastModified << "</a>";
            res << "<a>" << result.st_size << " B</a>";
        }
        res << "<a class='file-type'>";
        if (entry->d_type == DT_DIR)
            res << "Directory";
        else if (entry->d_type == DT_REG)
            res << "Regular file";
        else if (entry->d_type == DT_SOCK)
            res << "Socket file";
        else if (entry->d_type == DT_LNK)
            res << "Symlink";
        else if (entry->d_type == DT_BLK)
            res << "Block device";
        else
            res << "Unknown file type";
        res << "</a></li>";
    }
    res << "</ul></body><html>";

    closedir(dir);
    return res.str();
}

void ResponseGenerator::directoryResponse( ) {
    stringstream ss;
    string       html_res;

    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);

    ss << "HTTP/1.1 " << 200 << statusCodeMap->find(200)->second << CRLF;
    ss << "Date: " << date << CRLF;
    ss << "Content-Type: text/html" << CRLF;
    ss << "Connection: close" << CRLF;

    html_res = dirlisting();
    if (html_res == "")
        return ;

    ss << "Content-Length: " << html_res.length() << CRLF;

    ss << CRLF;
    ss << html_res;
    int a = send(clientFd, ss.str().c_str(), ss.str().length(), 0);

    if (a == -1) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
    }
    bond->reset();
}

void ResponseGenerator::POSTResponse( ) {
    stringstream ss;
    
    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);

    // Normal HTTP Header
    ss << "HTTP/1.1 " << 201 << statusCodeMap->find(201)->second << CRLF;
    ss << "Date: " << date << CRLF;
    ss << "Server: " <<  *(bond->getUri().getHostServer().getServerNames().begin()) << CRLF;

    // Normal HTTP Headers
    if (bond->getConnectionState()) ss << "Connection: keep-alive" << CRLF;
    else ss << "Connection: close" << CRLF;

    ss << CRLF;
    int a = send(clientFd, ss.str().c_str(), ss.str().length(), 0);

    if (a == -1) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
    }
    bond->reset();
}

int remove_resource(string &path) {
    if (remove(path.c_str())) {
        if (errno == EACCES)
            return 1; //403
        else if (errno == ENOENT)
            return 2; //404
        else 
            return 3; //500
    }
    return 0;
}

int delete_resource(string path, int is_dir) {
    if (!is_dir) {
        return remove_resource(path);
    }

    DIR* dir = opendir(path.c_str());
    struct dirent *entry = nullptr;
    struct stat result;

    if (path[path.size() - 1] != '/')
        path += "/";
    if (!dir)
    {
        if (errno == EACCES)
            return 1;
        else if (errno == ENOENT)
            return 2;
        return 3;
    }
    while((entry = readdir(dir)) != nullptr) {
        string file = entry->d_name; 
        string full_path = path + file;   
        if (file == "." || file == "..")
            continue;
        if (stat(full_path.c_str(), &result))
            return 3;
        int num = delete_resource(full_path, S_ISDIR(result.st_mode));
        if (num)
            return num;
    }
    closedir(dir);
    return remove_resource(path);
}

void ResponseGenerator::DELETEResponse( ) {
    struct stat result;
    if (false){ // Condition where i should check if it's a CGI case or not
        
    } else {
        if (stat(bond->getUri().path.c_str(), &result)) {
            this->exception = new RequestParser::HttpRequestException("Not found", 404);
            generateErrorMessage();
            return ;
        }
        int num = delete_resource(bond->getUri().path, S_ISDIR(result.st_mode));
        if (num) {
            if (num == 1)
                this->exception = new RequestParser::HttpRequestException("Forbidden", 403);
            else if (num == 2)
                this->exception = new RequestParser::HttpRequestException("Not found", 404);
            else
                this->exception = new RequestParser::HttpRequestException("Internal server error", 500);
            return;
        }
        stringstream ss;
        string       html_res;

        time_t timestamp = time(NULL);
        struct tm datetime1 = *localtime(&timestamp);
        char date[40];
        strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);

        ss << "HTTP/1.1 " << 200 << statusCodeMap->find(200)->second << CRLF;
        ss << "Date: " << date << CRLF;
        ss << "Connection: close" << CRLF;
        ss << CRLF;
        int a = send(clientFd, ss.str().c_str(), ss.str().length(), 0);
        if (a == -1) {
            this->exception = new RequestParser::HttpRequestException("System Error-Send Failed", 500);
            generateErrorMessage();
        }
        bond->reset();
    }
}

void ResponseGenerator::RedirectionResponse( ) {
    stringstream ss;
    
    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);

    // Normal HTTP Header
    ss << "HTTP/1.1 " << redirectPair.first << statusCodeMap->find(redirectPair.first)->second << CRLF;
    ss << "Date: " << date << CRLF;
    ss << "Location: " << redirectPair.second << CRLF;

    // Connection State
    if (bond->getConnectionState()) ss << "Connection: keep-alive" << CRLF;
    else ss << "Connection: close" << CRLF;
    
    ss << CRLF;

    int a = send(clientFd, ss.str().c_str(), ss.str().length(), 0);

    if (a == -1) {
        this->exception = new RequestParser::HttpRequestException("", 500);
        generateErrorMessage();
    }

    bond->reset();
}

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
    if (cgiExt == "py")
    {
        res = CheckExec("./cgi-exe/python-cgi");
        return res;
    }
    else if (cgiExt == "php")
    {
        res = CheckExec("./cgi-exe/php-cgi");
        if(res == "")
            res = CheckExec("/usr/bin/php");
        return res;
    }
    else if (cgiExt == "sh")
    {
        res = CheckExec("/bin/sh");
        return res;
    }
    else if (cgiExt == "pl")
    {
        res = CheckExec("/usr/bin/perl");
        return res;
    }
    return res;
}

void ResponseGenerator::CGI( ) {
    Uri& uri = bond->getUri();
    string cgiExec = getCgiExec(uri.getCgiExt());
    if (cgiExec == "")
    {
        this->exception = new RequestParser::HttpRequestException("Cgi executable not found or don't have the right permessions", 500);
        generateErrorMessage();
        return ;
    }
}

void ResponseGenerator::filterResponseType( ) {
    stringstream stream;
    string  responseBuffer;

    bond->rangeHeader();
    if (exception) generateErrorMessage();
    else {
        if (bond->isCGI()) CGI();
        else if (isRedirect) RedirectionResponse();
        else if (bond->getMethod() == GET) {
            if (bond->getUri().isDirectory()) directoryResponse();
            else if (bond->rangeHeader()) RangeGETResponse();
            else {
                NormalGETResponse();
            }
            return ;
        }
        else if (bond->getMethod() == POST) POSTResponse();
        else if (bond->getMethod() == DELETE) DELETEResponse();
    }
}
