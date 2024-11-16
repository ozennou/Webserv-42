/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:36:42 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/16 17:46:40 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RequestParser.hpp"
#include "../include/MessageHeaders.hpp"

RequestParser::RequestParser( int& clientFd, int& socketFd, Socket_map& socket_map, Bond* bond ) : uri(socketFd, socket_map) {
    this->bond = bond;
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

int  RequestParser::getMethod( void ) {
    return this->method;
}

Uri&  RequestParser::getUri( void ) {
    return uri;
}

void RequestParser::resolveResource( Location& location ) {

    // Either stat() failed, or the macro failed
    if (!uri.isRegularFile() && !uri.isDirectory()) throw RequestParser::HttpRequestException("The requested resource is neither a regular file or a directory, or it does not exists at all", 404);

    if (uri.isDirectory() && !location.getDirListings()) throw RequestParser::HttpRequestException("Directory Listing is off and the client is trying to access it", 403);

    if (uri.isRegularFile()) {
        if (method == GET) {
            if (access(uri.path.c_str(), R_OK) == -1) throw RequestParser::HttpRequestException("No permission to read the file", 403);
            set<string> set = location.getMethods();
            if (set.find("GET") == set.end()) throw RequestParser::HttpRequestException("Method is not allowed for this location", 403);
        }
    } else if (uri.isDirectory()) {
        cout << "It is a directory" << endl;
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

    if (count(requestLine.begin(), requestLine.end(), 32) != 2) throw  RequestParser::HttpRequestException("Wrong Number Of Spaces", 400);

    string requestMethod = requestLine.substr(0, requestLine.find(32));
    string targetURI = requestLine.substr(requestLine.find(32) + 1, (requestLine.find_last_of(32) - requestLine.find(32) - 1));
    string httpVersion = requestLine.substr(requestLine.find_last_of(32) + 1, (requestLine.length() - requestLine.find_last_of(32)));

    if (requestMethod.length() > 6 || requestMethod.length() <= 0) throw RequestParser::HttpRequestException("Bad Method - Invalid Length", 400);
    if (!targetURI.length()) throw RequestParser::HttpRequestException("Bad request-target - Length zero", 400);
    if (httpVersion.length() != 8 || httpVersion.find('/') != 4 \
    || httpVersion.compare(0, 6, "HTTP/1") || (httpVersion.compare(6, 2, ".0") && httpVersion.compare(6, 2, ".1")))
        throw RequestParser::HttpRequestException("Bad Http Version", 505);


    if (requestMethod == "GET") this->method = GET;
    else if (requestMethod == "POST") this->method = POST;
    else if (requestMethod == "DELETE") this->method = DELETE;
    else throw RequestParser::HttpRequestException("Invalid Method", 501);

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

    Location location = uri.matchURI(server);

    resolveResource(location);

    (void)bond;
}
