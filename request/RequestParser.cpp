#include "RequestParser.hpp"

RequestParser::RequestParser( int socketFd ) {
    this->socketFd = socketFd;
    size = 16;
}

RequestParser::HttpRequestException::HttpRequestException( string message, int statusCode ) {
    this->message = message;
    this->statusCode = statusCode;
}

const char* RequestParser::HttpRequestException::what( void ) const throw() {
    return message.c_str();
}

RequestParser::HttpRequestException::~HttpRequestException( ) throw() {
    
}

RequestParser::~RequestParser( ) {

}

void RequestParser::requestLine( string& stringBuffer, bool isEmpty ) {
    (void)isEmpty;
    int i;
    char buf[5160];

    // cout << "Test= " << stringBuffer << '\n';
    while ((i = recv(socketFd, buf, size, 0)))
    {
        buf[i] = 0;
        stringBuffer += buf;
        if (stringBuffer.find(CRLF) != string::npos) {
            break;
        }
    }
    cout << "--------------------\n";
    cout << stringBuffer << '\n';
    
}

void RequestParser::headerSection( char *buf, bool isEmpty ) {
    (void)isEmpty;
    (void)buf;
}

void RequestParser::fillRequestObject( ) {
    int i;
    char buf[5160];
    
    string stringBuffer;

    while ((i = recv(socketFd, buf, size, 0)))
    {
        buf[i] = 0;
        stringBuffer += buf;
        if (stringBuffer.find(CRLF) != string::npos) {
            break;
        }
    }
    cout << stringBuffer << '\n';
    string newStringBuffer = stringBuffer.substr(stringBuffer.find(CRLF) + 1, (stringBuffer.length() - stringBuffer.find(CRLF)));
    cout << "--------------------\n";
    cout << newStringBuffer << '\n';
    requestLine(newStringBuffer, false);
    // cout << stringBuffer;
    // requestLine(buf, true);
}