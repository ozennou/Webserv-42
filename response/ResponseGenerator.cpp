/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 20:52:22 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/19 21:13:25 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseGenerator.hpp"
#include "../include/Bond.hpp"

ResponseGenerator::ResponseGenerator( int clientFd, map<int, string>& statusCodeMap) : clientFd(clientFd), exception(NULL), statusCodeMap(statusCodeMap) {
    toRead = 0;
    reading = 0;
}

ResponseGenerator& ResponseGenerator::operator=( const ResponseGenerator& obj ) {
    if (this != &obj) {
        this->clientFd = obj.clientFd;
        this->toRead = obj.toRead;
        this->ifs = obj.ifs;
        this->exception = obj.exception;
        this->bond = obj.bond;
    }
    return *this;
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

    ss << "HTTP/1.1 " << exception->statusCode << " " << statusCodeMap.find(exception->statusCode)->second << CRLF;
    ss << "Date: " << date << CRLF;
    ss << "Server: " <<  *(uri.getHostServer().getServerNames().begin()) << CRLF;
    ss << "Connection: close" << CRLF;
    ss << "Content-Length: 0" << CRLF;
    ss << CRLF;
    responseBuffer += ss.str();

    int a = send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);

    if (a == -1) {
        ifs->close();
        cout << "Error:" << strerror(errno) << endl;
        // this->exception = new RequestParser::HttpRequestException("Send Failed", 500);
        // generateErrorMessage();
    }

    delete exception; exception = NULL;

    bond->setPhase(REQUEST_READY);
}

/*
        if toRead / 16000 >= 1  ---> 16000
        else toRead / 16000 < 1  ---> toRead
*/
void ResponseGenerator::completeRangeMessage( ) {
    string fileBuffer;
    size_t size = (toRead / 16000) >= 1 ? 16000 : toRead;
    char buf[size];

    ifs->read(buf, size);
    if (ifs->bad()) {
        ifs->close();
        cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("The Reading Of The File", 500);
        generateErrorMessage();
        return ;
    }

    toRead += ifs->gcount(); // maybe i could replace the gcount with just the size.
     
    fileBuffer.append(buf, ifs->gcount());

    int a = send(clientFd, fileBuffer.c_str(), fileBuffer.length(), 0);

    if (a == -1) {
        ifs->close();
        cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("Send Failed", 500);
        generateErrorMessage();
    }

    if (ifs->eof()) {
        bond->setResponseState(CLOSED);
        bond->setPhase(REQUEST_READY);
        ifs->close();
        return ;
    }
}

void ResponseGenerator::completeNormalMessage( ) {
    string fileBuffer;
    char buf[1621];

    ifs->read(buf, 1620);
    if (ifs->bad()) {
        ifs->close();
        cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("The Reading Of The File", 500);
        generateErrorMessage();
        return ;
    }

    fileBuffer.append(buf, ifs->gcount());

    int a = send(clientFd, fileBuffer.c_str(), fileBuffer.length(), 0);

    if (a == -1) {
        ifs->close();
        cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("Send Failed", 500);
        generateErrorMessage();
    }

    if (ifs->eof()) {
        bond->setResponseState(CLOSED);
        bond->setPhase(REQUEST_READY);
        ifs->close();
        return ;
    }
}

void ResponseGenerator::generateValidMessage( int statusCode, Uri& uri, string& contentType, string& fileBuffer ) {
    stringstream ss;
    
    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);
    
    // struct stat result;

    ss << "HTTP/1.1 " << statusCode << statusCodeMap.find(statusCode)->second << CRLF;
    ss << "Date: " << date << CRLF;
    // if (!stat(uri.path.c_str(), &result)) {
    //     struct tm datetime2 = *localtime(&result.st_mtime);
    //     char lastModified[40];
    //     strftime(lastModified, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime2);

    //     ss << "Last Modified: " << lastModified << CRLF;
    // }
    // ss << "Server: " <<  *(uri.getHostServer().getServerNames().begin()) << CRLF;
    ss << "Content-Type: " << contentType << CRLF;
    ss << "Content-Length: " << uri.getResourceSize() << CRLF;
    // ss << "Connection: keep-alive" << CRLF;
    // ss << "Accept-Ranges: none" << CRLF;

    // if (statusCode == 216) ss << "Accept-Ranges: bytes" << CRLF;
    
    ss << CRLF;

    if (fileBuffer.length()) ss << fileBuffer;

    int a = send(clientFd, ss.str().c_str(), ss.str().length(), 0);

    if (a == -1) {
        ifs->close();
        this->exception = new RequestParser::HttpRequestException("Send Failed", 500);
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
    
    ifs->open(uri.path.c_str(), ios_base::in | ios_base::binary);
    if (!ifs->is_open()) {
        cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("The Opening Of The File", 500);
        generateErrorMessage();
        bond->setPhase(REQUEST_READY);
        return ;
    }

    string fileBuffer;
    char buf[1621];
    
    ifs->read(buf, 1620);
    
    if (ifs->bad()) {
        ifs->close();
        cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("The Reading Of The File", 500);
        generateErrorMessage();
        bond->setPhase(REQUEST_READY);
        return ;
    }

    fileBuffer.append(buf, ifs->gcount());

    generateValidMessage(200, uri, contentType, fileBuffer);

    if (ifs->eof()) {
        bond->setResponseState(CLOSED);
        bond->setPhase(REQUEST_READY);
        ifs->close();
    } else {
        bond->setResponseState(OPEN);
    }
}

void ResponseGenerator::RangeGETResponse( ) {
    // if (bond->getResponseState() == OPEN) {
    //     completeRangeMessage();
    //     return ;
    // }

    Uri uri = bond->getUri();
    string contentType = "application/octet-stream";

    if (uri.path.rfind('.') != string::npos) {
        string tmp = uri.path.substr(uri.path.rfind('.') + 1);

        map<string, string> mimeTypes = uri.getHostServer().getMimeTypes();

        map<string, string>::iterator it = mimeTypes.find(tmp);

        if (it != mimeTypes.end()) contentType = it->second;
    }
    
    ifs->open(uri.path.c_str(), ios_base::in | ios_base::binary);
    if (!ifs->is_open()) {
        cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("The Opening Of The File", 500);
        generateErrorMessage();
        bond->setPhase(REQUEST_READY);
        return ;
    }
    
    if (bond->getRangeType() == INT_RANGE) {
        size_t rangeFirst = getRangeValue(bond->getRangeFirst());
        size_t rangeLast = getRangeValue(bond->getRangeLast());

        ifs->seekg(rangeFirst);

        toRead = rangeLast - rangeFirst;

    } else {
        size_t rangeLast = getRangeValue(bond->getRangeLast());

        ifs->seekg(rangeLast, ios_base::end);

        toRead = uri.getResourceSize() - rangeLast;
    }

    string fileBuffer;
    generateValidMessage(216, uri, contentType, fileBuffer);

    // if (ifs->eof()) {
    //     bond->setResponseState(CLOSED);
    //     bond->setPhase(REQUEST_READY);
    //     ifs->close();
    // } else {
    //     bond->setResponseState(OPEN);
    // }
}

void ResponseGenerator::filterResponseType( ) {
    stringstream stream;
    string  responseBuffer;

    if (exception)
        generateErrorMessage();
    else {
        if (bond->getMethod() == GET) {
            NormalGETResponse();
            // if (bond->isRange()) RangeGETResponse();
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

