/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:36:42 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/28 21:23:37 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RequestParser.hpp"
#include "../include/Headers.hpp"

RequestParser::RequestParser( int socketFd ) {
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

void RequestParser::requestLine( string& stringBuffer ) {

    if (!stringBuffer.length() || stringBuffer.find(CRLF) == string::npos) {
        int i;
        char buf[size];

        while((i = recv(socketFd, buf, size - 1, 0)) && i > 0) {
            buf[i] = 0; stringBuffer += buf;
            if (stringBuffer.find(CRLF) != string::npos)
                break;
        }
    }
    string requestLine = stringBuffer.substr(0, stringBuffer.find(CRLF));

    if (count(requestLine.begin(), requestLine.end(), 32) != 2) throw RequestParser::HttpRequestException("Invalid Request Line", 400);

    string methode = requestLine.substr(0, requestLine.find(32));
    string targetURI = requestLine.substr(requestLine.find(32) + 1, (requestLine.find_last_of(32) - requestLine.find(32) - 1));
    string httpVersion = requestLine.substr(requestLine.find_last_of(32) + 1, (requestLine.length() - requestLine.find_last_of(32)));

    if (methode.length() > 6 || methode.length() < 0) throw RequestParser::HttpRequestException("Invalid Methode", 400);
    if (!targetURI.length()) throw RequestParser::HttpRequestException("Invalid URI", 400);
    if (httpVersion.length() != 8 || httpVersion.find('/') != 4 \
    || httpVersion.compare(0, 6, "HTTP/1") || (httpVersion.compare(6, 2, ".0") && httpVersion.compare(6, 2, ".1")))
        throw RequestParser::HttpRequestException("Invalid HTTP-VERSION", 400);

    if (methode == "GET") request.methode = GET;
    else if (methode == "POST") request.methode = POST;
    else if (methode == "DELETE") request.methode = DELETE;
    else throw RequestParser::HttpRequestException("Invalid Methode", 400);

    Uri requestTarget(targetURI);

    headerSection(stringBuffer.substr(stringBuffer.find(CRLF) + 2));
    
}

void RequestParser::headerSection( string stringBuffer ) {
    Headers headerSection;

    while (1) {
        size_t pos = stringBuffer.find(CRLF);
        string field = stringBuffer.substr(0, pos);

        if (field == CRLF) break;
        
        pos += 2;
        headerSection.parseField(field);
        stringBuffer = stringBuffer.substr(pos);
    }
    // cout << "--------------------" << endl;
    // headerSection.print();
    // cout << "--------------------" << endl;
}

void RequestParser::fillRequestObject( ) {
    int i;
    char buf[size];
    string stringBuffer;

    i = recv(socketFd, buf, size - 1, 0);

    if (!i) throw RequestParser::HttpRequestException("Connection Ended", 0);
    if (i == -1) throw RequestParser::HttpRequestException("Nothing Yet", 1);

    buf[i] = 0; stringBuffer += buf;

    requestLine(stringBuffer);
    // cout << stringBuffer << endl;
    // string newStringBuffer = stringBuffer.substr(stringBuffer.find(CRLF) + 1, (stringBuffer.length() - stringBuffer.find(CRLF) - 1));
    // cout << "----------TTTTT----------\n";
    // cout << newStringBuffer << endl;
    // requestLine(newStringBuffer, false);
    // cout << stringBuffer;
    // requestLine(buf, true);
}