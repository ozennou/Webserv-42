/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 20:52:22 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/13 18:11:28 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseGenerator.hpp"
#include "../include/Bond.hpp"

ResponseGenerator::ResponseGenerator( int clientFd ) : exception(NULL), clientFd(clientFd) {
    errorHashMap.insert(make_pair<int, string>(505, "HTTP Version Not Supported"));
    errorHashMap.insert(make_pair<int, string>(501, "Not Implemented"));
    errorHashMap.insert(make_pair<int, string>(500, "Internal Server Error"));
    errorHashMap.insert(make_pair<int, string>(405, "Method Not Allowed"));
    errorHashMap.insert(make_pair<int, string>(404, "Not Found"));
    errorHashMap.insert(make_pair<int, string>(403, "Forbidden"));
    errorHashMap.insert(make_pair<int, string>(400, "Bad Request"));
}

void ResponseGenerator::setException( RequestParser::HttpRequestException* exc ) {
    this->exception = new RequestParser::HttpRequestException(*exc);
}

void ResponseGenerator::setBondObject( Bond* bondd ) {
    this->bond = bondd;
}

void ResponseGenerator::generateMessageError( ) {
    stringstream ss;
    Uri uri = bond->getUri();

    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);

    ss << "HTTP/1.1 " << exception->statusCode << " " << errorHashMap.find(exception->statusCode)->second << CRLF;
    ss << "Date: " << date << CRLF;
    ss << "Server: " <<  *(uri.getHostServer().getServerNames().begin()) << CRLF;
    ss << "Connection: close" << CRLF;
    ss << "Content-Length: 0" << CRLF;
    ss << CRLF;
    responseBuffer += ss.str();
    send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);
    delete exception; exception = NULL;
}

void ResponseGenerator::generateMessageValid( Uri& uri, string& contentType, string& fileBuffer ) {
    stringstream ss;
    
    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);
    
    struct stat result;

    ss << "HTTP/1.1 " << 200 << " OK" << CRLF;
    ss << "Date: " << date << CRLF;
    if (!stat(uri.path.c_str(), &result)) {
        struct tm datetime2 = *localtime(&result.st_mtime);
        char lastModified[40];
        strftime(lastModified, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime2);

        ss << "Last Modified: " << lastModified << CRLF;
    }
    ss << "Server: " <<  *(uri.getHostServer().getServerNames().begin()) << CRLF;
    ss << "Content-Type: " << contentType << CRLF;
    ss << "Content-Length: " << fileBuffer.length() << CRLF;
    ss << "Connection: close" << CRLF;
    ss << CRLF;

    ss << fileBuffer;
    responseBuffer += ss.str();
    
    int a = send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);
    responseBuffer = "";
    
    if (a == -1) {
        this->exception = new RequestParser::HttpRequestException("Send Failed", 500);
        generateMessageError();
    }
}

void ResponseGenerator::GETResponse( ) {
    Uri uri = bond->getUri();
    string contentType = "application/octet-stream";

    if (uri.path.find('.') != string::npos) {
        string tmp = uri.path.substr(uri.path.find('.') + 1);

        map<string, string> mimeTypes = uri.getHostServer().getMimeTypes();

        map<string, string>::iterator it = mimeTypes.find(tmp);

        if (it != mimeTypes.end()) contentType = it->second;
    }
    
    int fd = open(uri.path.c_str(), R_OK);
    if (fd == -1) {
        this->exception = new RequestParser::HttpRequestException("The Opening Of The File", 500);
        generateMessageError();
    }

    string fileBuffer;
    char buf[5160];

    int i = 0;
    while ((i = read(fd, buf, 5159), i != 0)) {
        if (i == -1) break;
        buf[i] = 0;
        fileBuffer += buf;
    }
    generateMessageValid(uri, contentType, fileBuffer);
}

void ResponseGenerator::filterResponseType( ) {
    stringstream stream;

    if (exception)
        generateMessageError();
    else {
        if (bond->getMethod() == GET) {
            GETResponse();
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
            responseBuffer = "";
        }
    }
}

ResponseGenerator::~ResponseGenerator( ) {
}
