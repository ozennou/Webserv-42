/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 20:52:22 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/25 11:54:09 by mlouazir         ###   ########.fr       */
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
        this->ifs = obj.ifs;
        this->exception = obj.exception;
        this->bond = obj.bond;
        this->statusCodeMap = obj.statusCodeMap;
    }
    return *this;
}

ResponseGenerator::ResponseGenerator( int clientFd, map<int, string>& statusCodeMap ) : clientFd(clientFd), exception(NULL), statusCodeMap(&statusCodeMap) {
    toRead = 0;
}

ResponseGenerator::~ResponseGenerator( ) {
    if (exception) delete exception;
}

void ResponseGenerator::setException( RequestParser::HttpRequestException* exc ) {
    this->exception = new RequestParser::HttpRequestException(*exc);
}

void ResponseGenerator::setBondObject( Bond* bondd ) {
    this->bond = bondd;
}

void ResponseGenerator::setInputStream( ifstream* ifss ) {
    this->ifs = ifss;
}

void ResponseGenerator::generateErrorMessage( ) {
    stringstream ss;
    string  responseBuffer;
    Uri uri = bond->getUri();

    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);

    ss << "HTTP/1.1 " << exception->statusCode << " " << statusCodeMap->find(exception->statusCode)->second << CRLF;
    ss << "Date: " << date << CRLF;
    ss << "Server: " <<  *(uri.getHostServer().getServerNames().begin()) << CRLF;
    ss << "Connection: close" << CRLF;
    ss << "Content-Length: 0" << CRLF;
    
    if (exception->statusCode == 416) ss << "Content-Range: */" << uri.getResourceSize() << CRLF;

    ss << CRLF;
    responseBuffer += ss.str();

    send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);

    logging(exception->message, WARNING, NULL, 0);

    delete exception; exception = NULL;

    bond->setPhase(REQUEST_READY);
    ifs->close();
}

void ResponseGenerator::completeRangeMessage( ) {
    string fileBuffer;
    size_t size = (toRead / 1620) >= 1 ? 1620 : toRead;
    char buf[size + 1];

    ifs->read(buf, size);
    if (ifs->bad()) {
        // cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("System Error-The Reading Of The File", 500);
        generateErrorMessage();
        return ;
    }

    toRead -= ifs->gcount();
     
    fileBuffer.append(buf, ifs->gcount());

    int a = send(clientFd, fileBuffer.c_str(), fileBuffer.length(), 0);

    if (a == -1) {
        // cout << "Send-Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("System Error-Send Failed", 500);
        generateErrorMessage();
        return ;
    }

    if (!toRead) {
        bond->setResponseState(CLOSED);
        bond->setPhase(REQUEST_READY);
        ifs->close();
        return ;
    }

}

void ResponseGenerator::completeNormalMessage( ) {
    string fileBuffer;
    char buf[1621];

    buf[0] = 0;
    ifs->read(buf, 1620);
    if (ifs->bad()) {
        // cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("System Error-The Reading Of The File", 500);
        generateErrorMessage();
        return ;
    }

    fileBuffer.append(buf, ifs->gcount());

    int a = send(clientFd, fileBuffer.c_str(), fileBuffer.length(), 0);

    if (a == -1) {
        // cout << "Send-Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("System Error-Send Failed", 500);
        generateErrorMessage();
        return ;
    }

    if (ifs->eof()) {
        bond->setResponseState(CLOSED);
        bond->setPhase(REQUEST_READY);
        ifs->close();
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
    
    // cout << ss.str() << endl;
    int a = send(clientFd, ss.str().c_str(), ss.str().length(), 0);

    if (a == -1) {
        cout << "Send-Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("System Error-Send Failed", 500);
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

    ifs->open(uri.path.c_str(), ios::in | ios::binary);
    if (!ifs->is_open()) {
        // cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("System Error-The Opening Of The File", 500);
        generateErrorMessage();
        return ;
    }

    string fileBuffer;
    char buf[1621];
    
    ifs->read(buf, 1620);
    
    if (ifs->bad()) {
        cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("System Error-The Reading Of The File", 500);
        generateErrorMessage();
        return ;
    }

    fileBuffer.append(buf, ifs->gcount());

    generateValidMessage(200, uri, contentType, fileBuffer, -1, -1);

    if (ifs->eof()) {
        bond->setResponseState(CLOSED);
        bond->setPhase(REQUEST_READY);
        ifs->close();
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
    
    ifs->open(uri.path.c_str(), ios::in | ios::binary);
    if (!ifs->is_open()) {
        // cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("System Error-Opening Of The Resource", 500);
        generateErrorMessage();
        return ;
    }
    
    size_t rangeFirst;
    size_t rangeLast;
    if (bond->getRangeType() == INT_RANGE) {
        rangeFirst = getRangeValue(bond->getRangeFirst());
        rangeLast = getRangeValue(bond->getRangeLast());
        
        if (!rangeLast || rangeLast >= uri.getResourceSize()) rangeLast = uri.getResourceSize() - 1;

        ifs->seekg(rangeFirst);

        toRead = rangeLast - rangeFirst + 1;
    } else {
        rangeFirst = getRangeValue(bond->getRangeLast());
        rangeLast = uri.getResourceSize() - 1;

        ifs->seekg(rangeLast - rangeFirst);

        toRead = rangeLast - (rangeLast - rangeFirst);
    }

    string fileBuffer;
    size_t size = (toRead / 1620) >= 1 ? 1620 : toRead;
    char buf[size + 1];
    
    ifs->read(buf, size);
    if (ifs->bad()) {
        this->exception = new RequestParser::HttpRequestException("System Error-The Reading Of The File", 500);
        generateErrorMessage();
        return ;
    }

    toRead -= ifs->gcount();
     
    fileBuffer.append(buf, ifs->gcount());

    generateValidMessage(206, uri, contentType, fileBuffer, rangeFirst, rangeLast);

    if (!toRead) {
        bond->setResponseState(CLOSED);
        bond->setPhase(REQUEST_READY);
        ifs->close();
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
    res << ".file-list { font-weight: 300; list-style-type: none; padding: 0; margin-top: 20px; }";
    res << "li { display: flex; justify-content: space-between; margin: 5px 0; padding: 5px 0; }";
    res << ".file-name { font-weight: bold; text-decoration: none; color: #007bff; flex: 1; }";
    res << ".test { font-weight: bold; }";

    res << ".file-type { font-style: italic; color: gray; text-align: right; margin-left: 10px; }";
    res << "</style></head><body>";

    res << "<h1>Index of " << uri.requestTarget << "</h1>";
    res << "<ul class='file-list'>";
    res << "<ul class='file-list'>";
    res << "<li class='test'><a>File name</a><a>Last modified</a><a>size</a><a>File type</a></li>";
    while ((entry = readdir(dir)) != nullptr) {
        string full_path = uri.path + string(entry->d_name);

        if (entry->d_name[0] == '.' && entry->d_name[1] == '\0')
            continue ;
        res << "<li><a href='"<< entry->d_name << "'>" << entry->d_name << "</a>";
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
        this->exception = new RequestParser::HttpRequestException("System Error-Send Failed", 500);
        generateErrorMessage();
    }
}

void ResponseGenerator::filterResponseType( ) {
    stringstream stream;
    string  responseBuffer;

    bond->rangeHeader();
    if (exception) generateErrorMessage();
    else {
        if (bond->getMethod() == GET) {
            if (bond->getUri().isDirectory()) directoryResponse();
            else if (bond->rangeHeader()) RangeGETResponse();
            else NormalGETResponse();
            return ;
        }
        else {
            stream << "HTTP/1.1 " << 200 << " OK" << CRLF;
            stream << "Date: Thu, 16 Nov 2017 16:40:10 GMT" << endl;
            stream << "Content-Length: 1" << CRLF;
            stream << CRLF;
            stream << "H";
            stream << "";
            responseBuffer += stream.str();
            send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);
        }
    }
}

