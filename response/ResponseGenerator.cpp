/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 20:52:22 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/22 18:07:43 by mlouazir         ###   ########.fr       */
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
    // cout << "ResponseGenerator Copy" << endl;
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
    reading = 0;
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

    cout << "Error Message" << endl;
    time_t timestamp = time(NULL);
    struct tm datetime1 = *localtime(&timestamp);
    char date[40];
    strftime(date, 40, "%a, %d %b %Y %H:%M:%S GMT", &datetime1);

    ss << "HTTP/1.1 " << exception->statusCode << " " << statusCodeMap->find(exception->statusCode)->second << CRLF;
    ss << "Date: " << date << CRLF;
    ss << "Server: " <<  *(uri.getHostServer().getServerNames().begin()) << CRLF;
    ss << "Connection: close" << CRLF;
    ss << "Content-Length: 0" << CRLF;
    ss << CRLF;
    responseBuffer += ss.str();

    int a = send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);

    if (a == -1) {
        ifs->close();
        cout << "Send-Error:" << strerror(errno) << endl;
    }

    delete exception; exception = NULL;

    bond->setPhase(REQUEST_READY);
}

void ResponseGenerator::completeRangeMessage( ) {
    string fileBuffer;
    size_t size = (toRead / 1620) >= 1 ? 1620 : toRead;
    char buf[size + 1];

    ifs->read(buf, size);
    if (ifs->bad()) {
        ifs->close();
        cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("The Reading Of The File", 500);
        generateErrorMessage();
        return ;
    }

    toRead -= ifs->gcount();
     
    fileBuffer.append(buf, ifs->gcount());

    int a = send(clientFd, fileBuffer.c_str(), fileBuffer.length(), 0);

    if (a == -1) {
        ifs->close();
        cout << "Send-Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("Send Failed", 500);
        generateErrorMessage();
    }

    if (!toRead) {
        logging("Completed", WARNING, NULL, 0);
        bond->setResponseState(CLOSED);
        bond->setPhase(REQUEST_READY);
        ifs->close();
        return ;
    }

}

void ResponseGenerator::completeNormalMessage( ) {
    string fileBuffer;
    char buf[1621];

    if (!ifs) cout << "SEGV" << endl;
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
        cout << "Send-Error:" << strerror(errno) << endl;
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

void ResponseGenerator::generateValidMessage( int statusCode, Uri& uri, string& contentType, string& fileBuffer, size_t rangeFirst, size_t rangeLast ) {
    stringstream ss;
    
    (void)fileBuffer;
    (void)rangeFirst;
    (void)rangeLast;
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
        ss << "Content-Length: " << rangeLast - rangeFirst + 1 << CRLF;
    }
    else ss << "Content-Length: " << uri.getResourceSize() << CRLF;

    // Normal HTTP Header
    ss << "Connection: keep-alive" << CRLF;

    // In Case of Range request
    // if (bond->isRange()) ss << "Accept-Ranges: bytes" << CRLF;
    
    ss << CRLF;

    // Adding The Paylod
    ss << fileBuffer;
    
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
    cout << "*" << endl;
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
    
    cout << "@" << endl;
    ifs->open(uri.path.c_str(), ios_base::in | ios_base::binary);
    if (!ifs->is_open()) {
        cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("The Opening Of The File", 500);
        generateErrorMessage();
        bond->setPhase(REQUEST_READY);
        return ;
    }
    
    size_t rangeFirst;
    size_t rangeLast;
    if (bond->getRangeType() == INT_RANGE) {
        rangeFirst = getRangeValue(bond->getRangeFirst());
        rangeLast = getRangeValue(bond->getRangeLast());
        
        if (!rangeLast || rangeLast > uri.getResourceSize()) rangeLast = uri.getResourceSize() - 1;

        ifs->seekg(rangeFirst);

        toRead = rangeLast - rangeFirst + 1; reading = 0;
    } else {
        rangeFirst = getRangeValue(bond->getRangeLast());
        rangeLast = uri.getResourceSize() - 1;

        ifs->seekg(rangeFirst, ios_base::end);

        toRead = uri.getResourceSize() - rangeLast + 1; reading = 0;\

    }

    string fileBuffer;
    size_t size = (toRead / 1620) >= 1 ? 1620 : toRead;
    char buf[size + 1];
    
    ifs->read(buf, size);
    if (ifs->bad()) {
        ifs->close();
        cout << "Error:" << strerror(errno) << endl;
        this->exception = new RequestParser::HttpRequestException("The Reading Of The File", 500);
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

void ResponseGenerator::filterResponseType( ) {
    stringstream stream;
    string  responseBuffer;

    if (exception)
        generateErrorMessage();
    else {
        if (bond->getMethod() == GET) {
            // if (bond->isRange()) RangeGETResponse();
            // else NormalGETResponse();
            NormalGETResponse();
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

