/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:36:42 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/01 15:42:48 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RequestParser.hpp"
#include "../include/MessageHeaders.hpp"

RequestParser::RequestParser( int socketFd, RequestMessage* requestMessage ) : requestMessage(requestMessage) {
    this->socketFd = socketFd;
    size = 5160;
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

void RequestParser::findCRLF( string& stringBuffer) {
    if (!stringBuffer.length() || stringBuffer.find(CRLF) == string::npos) {
        int i;
        char buf[size];

        while((i = recv(socketFd, buf, size - 1, 0)) && i > 0) {
            buf[i] = 0; stringBuffer += buf;
            if (stringBuffer.find(CRLF) != string::npos)
                break;
        }
    }
}

void RequestParser::requestLine( string& stringBuffer ) {

    findCRLF(stringBuffer);

    string requestLine = stringBuffer.substr(0, stringBuffer.find(CRLF));

    if (count(requestLine.begin(), requestLine.end(), 32) != 2) throw RequestParser::HttpRequestException("Wrong Number of Spaces Than It Should Be", 400);

    string methode = requestLine.substr(0, requestLine.find(32));
    string targetURI = requestLine.substr(requestLine.find(32) + 1, (requestLine.find_last_of(32) - requestLine.find(32) - 1));
    string httpVersion = requestLine.substr(requestLine.find_last_of(32) + 1, (requestLine.length() - requestLine.find_last_of(32)));

    if (methode.length() > 6 || methode.length() <= 0) throw RequestParser::HttpRequestException("Invalid Methode", 400);
    if (!targetURI.length()) throw RequestParser::HttpRequestException("Invalid URI", 400);
    if (httpVersion.length() != 8 || httpVersion.find('/') != 4 \
    || httpVersion.compare(0, 6, "HTTP/1") || (httpVersion.compare(6, 2, ".0") && httpVersion.compare(6, 2, ".1")))
        throw RequestParser::HttpRequestException("Invalid HTTP-VERSION", 400);

    if (methode == "GET") requestMessage->methode = GET;
    else if (methode == "POST") requestMessage->methode = POST;
    else if (methode == "DELETE") requestMessage->methode = DELETE;
    else throw RequestParser::HttpRequestException("Invalid Methode", 400);

    Uri* requestTarget = new Uri(targetURI);

    requestMessage->uri = requestTarget;

    headerSection(stringBuffer.substr(stringBuffer.find(CRLF) + 2));
    
    cout << requestMessage->methode << endl;
    if (requestMessage->uri->host.length()) cout << requestMessage->uri->host << endl;
    if (requestMessage->uri->port.length()) cout << requestMessage->uri->port << endl;    
    if (requestMessage->uri->path.length()) cout << requestMessage->uri->path << endl;
    if (requestMessage->uri->query.length()) cout << requestMessage->uri->query << endl;
    requestMessage->headers->print();
}

void RequestParser::headerSection( string stringBuffer ) {

    findCRLF(stringBuffer);
    
    MessageHeaders *headerSection = new MessageHeaders();

    while (1) {
        size_t pos = stringBuffer.find(CRLF);
        string field = stringBuffer.substr(0, pos);

        if (!field.length()) break;
        
        pos += 2;
        headerSection->parseFieldName(field);
        stringBuffer = stringBuffer.substr(pos);
    }

    requestMessage->headers = headerSection;

    headerSection->parseFieldValue();
}

void RequestParser::init( ) {
    int i;
    char buf[size];
    string stringBuffer;

    i = recv(socketFd, buf, size - 1, 0);

    if (!i) throw RequestParser::HttpRequestException("Connection Ended", 0);
    if (i == -1) throw RequestParser::HttpRequestException("Nothing Yet", 1);

    buf[i] = 0; stringBuffer += buf;

    requestLine(stringBuffer);
}