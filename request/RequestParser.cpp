/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:36:42 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/08 14:27:44 by mlouazir         ###   ########.fr       */
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

void RequestParser::requestLine( string& stringBuffer ) {

    findCRLF(stringBuffer);

    string requestLine = stringBuffer.substr(0, stringBuffer.find(CRLF));

    if (count(requestLine.begin(), requestLine.end(), 32) != 2) throw  RequestParser::HttpRequestException("Bad Request", 400);

    string methode = requestLine.substr(0, requestLine.find(32));
    string targetURI = requestLine.substr(requestLine.find(32) + 1, (requestLine.find_last_of(32) - requestLine.find(32) - 1));
    string httpVersion = requestLine.substr(requestLine.find_last_of(32) + 1, (requestLine.length() - requestLine.find_last_of(32)));

    if (methode.length() > 6 || methode.length() <= 0) throw RequestParser::HttpRequestException("Bad Request", 400);
    if (!targetURI.length()) throw RequestParser::HttpRequestException("Bad Request", 400);
    if (httpVersion.length() != 8 || httpVersion.find('/') != 4 \
    || httpVersion.compare(0, 6, "HTTP/1") || (httpVersion.compare(6, 2, ".0") && httpVersion.compare(6, 2, ".1")))
        throw RequestParser::HttpRequestException("Bad Request", 400);


    if (methode == "GET") this->methode = GET;
    else if (methode == "POST") this->methode = POST;
    else if (methode == "DELETE") this->methode = DELETE;
    else throw RequestParser::HttpRequestException("Bad Request", 400);

    uri.extractPath(targetURI);

    uri.normalizePath();

    headerSection(stringBuffer.substr(stringBuffer.find(CRLF) + 2));
}

void RequestParser::headerSection( string stringBuffer ) {

    findCRLF(stringBuffer);

    while (1) {
        size_t pos = stringBuffer.find(CRLF);
        string field = stringBuffer.substr(0, pos);

        if (!field.length()) break;
        
        pos += 2;
        headers.parseFieldName(field, uri);
        stringBuffer = stringBuffer.substr(pos);
    }

    headers.parseFieldValue();
}

void RequestParser::checkTargetUri( Server& server ) {
    Location location;
    vector<Location> locations = server.getLocations();

    // Getting the exact path Of the resource
    for (vector<Location>::iterator it = locations.begin(); it != locations.end(); it++) {
        if ((uri.path == it->getRoute()) \
        || ((!uri.path.rfind(it->getRoute(), 0)) \
            && (it->getRoute().length() > location.getRoute().length()) \
            && (uri.path[it->getRoute().length()] == '/' || it->getRoute()[it->getRoute().length() - 1] == '/'))) {
            location = *it;
        }
    }

    if (!location.getRoute().length()) throw RequestParser::HttpRequestException("Not Found", 404);

    uri.path.insert(0, location.getRoot());

    // Verifying Methods
    set<string> methodsAllowed = location.getMethods();

    if ((methode == GET && methodsAllowed.find("GET") == methodsAllowed.end()) \
    || (methode == POST && methodsAllowed.find("POST") == methodsAllowed.end()) \
    || (methode == DELETE && methodsAllowed.find("DELETE") == methodsAllowed.end()))
        throw RequestParser::HttpRequestException("Method Not Allowed", 405);
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
    
    Server server = uri.checkHostInfo();

    checkTargetUri(server);


    if (methode == GET) cout << "GET" << endl;
    else if (methode == POST) cout << "POST" << endl;
    else if (methode == DELETE) cout << "DELETE" << endl;
    else cout << "No Header Found" << endl;

    if (uri.host.length()) cout << "Host= "<< uri.host << endl;
    else cout << "Host Not Found" << endl;

    cout << "Port="<< uri.port << endl;

    if (uri.path.length()) cout << "Path= "<< uri.path << endl;

    if (uri.query.length()) cout << uri.query << endl;
    else cout << "No Query" << endl;

    cout << "Resource URI= "<< uri.path << endl;
}