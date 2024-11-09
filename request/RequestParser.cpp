/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:36:42 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/09 14:22:02 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RequestParser.hpp"
#include "../include/MessageHeaders.hpp"

RequestParser::RequestParser( int clientFd, int socketFd, Socket_map& socket_map ) : uri(socketFd, socket_map) {
    this->clientFd = clientFd;
    size = 5160;
}

RequestParser::HttpRequestException::HttpRequestException( string message, int statusCode ) {
    this->message = message;
    this->statusCode = statusCode;
}

RequestParser::HttpRequestException::HttpRequestException( const HttpRequestException& obj ) {
    if (this != &obj) {
        this->message = obj.message;
        this->statusCode = obj.statusCode;
    }
}

const char* RequestParser::HttpRequestException::what( void ) const throw() {
    return message.c_str();
}

RequestParser::HttpRequestException::~HttpRequestException( ) throw() {
}

RequestParser::~RequestParser( ) {
}

void RequestParser::findCRLF( string& stringBuffer ) {
    if (!stringBuffer.length() || stringBuffer.find(CRLF) == string::npos) {
        int i;
        char buf[size];

        while((i = recv(clientFd, buf, size - 1, 0)) && i > 0) {
            buf[i] = 0; stringBuffer += buf;
            if (stringBuffer.find(CRLF) != string::npos)
                break;
        }
    }
}

void RequestParser::headerSection( string stringBuffer ) {

    findCRLF(stringBuffer);

    while (1) {
        size_t pos = stringBuffer.find(CRLF);
        string field = stringBuffer.substr(0, pos);

        if (!field.length()) break;
        
        pos += 2;
        headers.storeField(field, uri);
        stringBuffer = stringBuffer.substr(pos);
    }

    headers.parceFieldValue();
}

void RequestParser::requestLine( string& stringBuffer ) {

    findCRLF(stringBuffer);

    string requestLine = stringBuffer.substr(0, stringBuffer.find(CRLF));

    if (count(requestLine.begin(), requestLine.end(), 32) != 2) throw  RequestParser::HttpRequestException("Bad Request", 400);

    string requestMethod = requestLine.substr(0, requestLine.find(32));
    string targetURI = requestLine.substr(requestLine.find(32) + 1, (requestLine.find_last_of(32) - requestLine.find(32) - 1));
    string httpVersion = requestLine.substr(requestLine.find_last_of(32) + 1, (requestLine.length() - requestLine.find_last_of(32)));

    if (requestMethod.length() > 6 || requestMethod.length() <= 0) throw RequestParser::HttpRequestException("Bad Request", 400);
    if (!targetURI.length()) throw RequestParser::HttpRequestException("Bad Request", 400);
    if (httpVersion.length() != 8 || httpVersion.find('/') != 4 \
    || httpVersion.compare(0, 6, "HTTP/1") || (httpVersion.compare(6, 2, ".0") && httpVersion.compare(6, 2, ".1")))
        throw RequestParser::HttpRequestException("Bad Request", 400);


    if (requestMethod == "GET") this->method = GET;
    else if (requestMethod == "POST") this->method = POST;
    else if (requestMethod == "DELETE") this->method = DELETE;
    else throw RequestParser::HttpRequestException("Bad Request", 400);

    uri.extractPath(targetURI);
}

void RequestParser::init( ) {
    int i;
    char buf[size];
    string stringBuffer;

    i = recv(clientFd, buf, size, 0);

    if (!i) throw RequestParser::HttpRequestException("Connection Ended", 0);
    if (i == -1) throw RequestParser::HttpRequestException("Nothing Yet", -1);

    buf[i] = 0; stringBuffer += buf;

    requestLine(stringBuffer);
    
    headerSection(stringBuffer.substr(stringBuffer.find(CRLF) + 2));

    Server server = uri.getHostServer();

    uri.matchURI(server);

    if (method == GET) cout << "GET" << endl;
    else if (method == POST) cout << "POST" << endl;
    else if (method == DELETE) cout << "DELETE" << endl;
    else cout << "No Header Found" << endl;

    if (uri.host.length()) cout << "Host= "<< uri.host << endl;
    else cout << "Host Not Found" << endl;

    cout << "Port="<< uri.port << endl;

    if (uri.path.length()) cout << "Path= "<< uri.path << endl;

    if (uri.query.length()) cout << uri.query << endl;
    else cout << "No Query" << endl;

    cout << "Resource URI= "<< uri.path << endl;
}