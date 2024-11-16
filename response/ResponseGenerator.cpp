/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 20:52:22 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/16 21:02:15 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseGenerator.hpp"
#include "../include/Bond.hpp"

ResponseGenerator::ResponseGenerator( int clientFd, map<int, string>& statusCodeMap) : clientFd(clientFd), exception(NULL), statusCodeMap(statusCodeMap) {
    size = 0;
}

ResponseGenerator& ResponseGenerator::operator=( const ResponseGenerator& obj ) {
    if (this != &obj) {
    }
    return *this;
}

ResponseGenerator::~ResponseGenerator( ) {
    if (exception) delete exception;
    fileStream->close(); delete fileStream;
}

void ResponseGenerator::setException( RequestParser::HttpRequestException* exc ) {
    this->exception = new RequestParser::HttpRequestException(*exc);
}

void ResponseGenerator::setBondObject( Bond* bondd ) {
    this->bond = bondd;
}

void ResponseGenerator::generateErrorMessage( ) {
    stringstream ss;
    string  responseBuffer;
    Uri uri = bond->getUri();

    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);

    cout << "**->"<< exception->message << endl;
    ss << "HTTP/1.1 " << exception->statusCode << " " << statusCodeMap.find(exception->statusCode)->second << CRLF;
    ss << "Date: " << date << CRLF;
    ss << "Server: " <<  *(uri.getHostServer().getServerNames().begin()) << CRLF;
    ss << "Connection: close" << CRLF;
    ss << "Content-Length: 0" << CRLF;
    ss << CRLF;
    responseBuffer += ss.str();

    send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);
    delete exception; exception = NULL;
}

void ResponseGenerator::completeMessage( ) {
    string fileBuffer;
    char buf[16001];

    fileStream->read(buf, 1600);
    if (fileStream->bad() || fileStream->fail()) {
        fileStream->close();
        perror("Message From perror:");
        this->exception = new RequestParser::HttpRequestException("The Reading Of The File", 500);
        generateErrorMessage();
        return ;
    }

    if (!fileStream->gcount()) {
        bond->setResponseState(CLOSED);
        bond->setPhase(REQUEST_READY);
        fileStream->close();
        return ;
    }

    fileBuffer.append(buf, fileStream->gcount());

    int a = send(clientFd, fileBuffer.c_str(), fileBuffer.length(), 0);

    if (a == -1) {
        fileStream->close();
        perror("Message From perror:");
        this->exception = new RequestParser::HttpRequestException("Send Failed", 500);
        generateErrorMessage();
    }
}

void ResponseGenerator::generateValidMessage( Uri& uri, string& contentType, string& fileBuffer ) {
    string      responseBuffer;
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
    ss << "Content-Length: " << uri.getResourceSize() << CRLF;
    ss << "Connection: keep-alive" << CRLF;
    ss << CRLF;

    ss << fileBuffer;
    responseBuffer += ss.str();
    
    int a = send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);

    if (a == -1) {
        fileStream->close();
        this->exception = new RequestParser::HttpRequestException("Send Failed", 500);
        generateErrorMessage();
    }
}

void ResponseGenerator::GETResponse( ) {
    if (bond->getResponseState() == OPEN) {
        completeMessage();
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
    
    fileStream->open(uri.path.c_str());
    if (!fileStream->is_open()) {
        perror("Message From perror:");
        this->exception = new RequestParser::HttpRequestException("The Opening Of The File", 500);
        generateErrorMessage();
        return ;
    }

    string fileBuffer;
    char buf[16001];

    fileStream->read(buf, 1600);
    if (fileStream->bad() || fileStream->fail()) {
        fileStream->close();
        perror("Message From perror:");
        this->exception = new RequestParser::HttpRequestException("The Reading Of The File", 500);
        generateErrorMessage();
        return ;
    }

    fileBuffer.append(buf, fileStream->gcount());

    generateValidMessage(uri, contentType, fileBuffer);

    if (fileStream->eof()) {
        bond->setResponseState(CLOSED);
        bond->setPhase(REQUEST_READY);
        fileStream->close();
    } else {
        bond->setResponseState(OPEN);
    }
}

void ResponseGenerator::filterResponseType( ) {
    stringstream stream;
    string  responseBuffer;

    if (exception)
        generateErrorMessage();
    else {
        if (bond->getMethod() == GET) {
            GETResponse();
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

