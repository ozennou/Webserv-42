/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:36:42 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/25 15:11:00 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RequestParser.hpp"
#include "../include/MessageHeaders.hpp"

RequestParser::RequestParser( ) {
}

RequestParser::RequestParser( const RequestParser& obj ) {
    *this = obj;
}

RequestParser& RequestParser::operator=( const RequestParser& obj ) {
    if (this != &obj) {
        this->bond = obj.bond;
        this->method = obj.method;
        this->uri = obj.uri;
        this->headers = obj.headers;
        this->size = 5160;
        this->payload = obj.payload;
        this->clientFd = obj.clientFd;
    }
    return *this;
}

RequestParser::RequestParser( int& clientFd, int& socketFd, Socket_map& socket_map ) : uri(socketFd, socket_map) {
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
        char buf[size + 1];

        while((i = recv(clientFd, buf, size, 0)) && i > 0) {
            stringBuffer.append(buf, i);
            if (stringBuffer.find(CRLF) != string::npos)
                break;
        }
    }
}

void RequestParser::setBondObject( Bond* bondd ) {
    this->bond = bondd;
}

int  RequestParser::getMethod( void ) {
    return this->method;
}

Uri&  RequestParser::getUri( void ) {
    return uri;
}

bool  RequestParser::isRange( void ) {
    return headers.findField("range");
}

bool  RequestParser::isValidRange( void ) {
    return headers.isValidRange(uri);
}

string  RequestParser::getRangeFirst( void ) {
    return headers.getRangeFirst();
}

string  RequestParser::getRangeLast( void ) {
    return headers.getRangeLast();
}

int  RequestParser::getRangeType( void ) {
    return headers.getRangeType();
}

bool  RequestParser::getConnectionState( void ) {
    return headers.connectionState();
}

void RequestParser::resolveResource( Location& location ) {

    // Either stat() failed, or the macro failed
    if (!uri.isRegularFile() && !uri.isDirectory()) throw RequestParser::HttpRequestException("The requested resource is neither a regular file or a directory, or it does not exists at all", 404);

    if (uri.isDirectory()) {
        vector<string> defaultPages = location.getDefaultPages();
        vector<string>::iterator iterator = defaultPages.begin();

        for (; iterator != defaultPages.end(); iterator++) {
            string concatenated = uri.path;
            concatenated += *(iterator);

            if (uri.isRegularFile(concatenated)) {
                uri.path = concatenated;
                break;
            }
        }

        if (iterator == defaultPages.end() && !location.getDirListings()) throw RequestParser::HttpRequestException("Directory Listing is off and the client is trying to access it", 403);
    }

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
    
    while (1) {
        if (stringBuffer.find(CRLF) == string::npos) findCRLF(stringBuffer);

        size_t pos = stringBuffer.find(CRLF);
        string field = stringBuffer.substr(0, pos);

        if (!field.length()) break;
        
        pos += 2;
        headers.storeField(field, uri, method);
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
    char buf[size + 1];
    string stringBuffer;

    i = recv(clientFd, buf, size, 0);

    if (!i) throw RequestParser::HttpRequestException("Connection Ended", 0);
    if (i == -1) throw RequestParser::HttpRequestException("Nothing Yet", -1);

    stringBuffer.append(buf, i);
    
    requestLine(stringBuffer);
    
    headerSection(stringBuffer.substr(stringBuffer.find(CRLF) + 2));

    Server server = uri.getHostServer();

    Location location = uri.matchURI(server);

    resolveResource(location);
}
