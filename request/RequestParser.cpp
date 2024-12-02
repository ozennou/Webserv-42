/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:36:42 by mlouazir          #+#    #+#             */
/*   Updated: 2024/12/01 13:16:10 by mlouazir         ###   ########.fr       */
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
        this->uploader = obj.uploader;
        this->size = 5160;
        this->stringBuffer = obj.stringBuffer;
        this->clientFd = obj.clientFd;
    }
    return *this;
}

RequestParser::RequestParser( int& clientFd, int& socketFd, Socket_map& socket_map ) : uploader(clientFd), uri(socketFd, socket_map) {
    this->method = -1;
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

void RequestParser::reset( ) {
    method = -1;
    stringBuffer.clear();
    uploader.reset();
    uri.reset();
    headers.reset();
}

void RequestParser::findCRLF( ) {
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

string&  RequestParser::getStringBuffer( void ) {
    return this->stringBuffer;
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

int  RequestParser::getUploadState( void ) {
    return uploader.getUploadState();
}

bool  RequestParser::getConnectionState( void ) {
    return headers.connectionState();
}

void  RequestParser::upload( void ) {
    uploader.read();
}

void  RequestParser::setUploader( Server& server, Location& location ) {

    uploader.setIsChunked(false);
    uploader.setIsMulti(false);

    string contentType = headers.getFieldValue("content-type");
    map<string, string> mapp = server.getMimeTypes();
    map<string, string>::iterator it = mapp.begin();

    for (; it != mapp.end(); it++) {
        if (contentType == it->second) break;
    }
    
    if (it != mapp.end()) uploader.setFileType(it->first);
    else if (contentType.rfind("multipart/form-data", 0) != string::npos) {
        uploader.setIsMulti(true);
        uploader.setBoundary(contentType.substr(contentType.find('=') + 1));
    }

    uploader.setMaxPayloadSize(server.getBodySize());
    
    if (headers.getFieldValue("transfer-encoding") == "chunked") uploader.setIsChunked(true);
    else {
        stringstream ss;
        size_t sizee;
        if (!headers.getFieldValue("content-length").length()) throw  RequestParser::HttpRequestException("Invalid Content-Length Value", 400);
        ss << headers.getFieldValue("content-length"); ss >> sizee;
        if (ss.fail()) throw  RequestParser::HttpRequestException("Invalid Content-Length Value", 400);

        uploader.setTotalLength(sizee);
    }

    uploader.setOfs(location.getUploadPath());

    uploader.setUploadState(UPLOADING);
    
    uploader.setBuffer(stringBuffer);
}

void RequestParser::resolveResource( Location& location ) {

    // In Case Of POST method
    if (method == POST) {
        if (access(location.getUploadPath().c_str(), W_OK) == -1) throw RequestParser::HttpRequestException("No permission to write to the directory", 403);
        set<string> set = location.getMethods();
        if (set.find("POST") == set.end()) throw RequestParser::HttpRequestException("Method is not allowed for this location", 403);
        headers.findContentHeaders();
        return ;
    }

    // Either stat() failed, or the macro failed
    if (!uri.isRegularFile() && !uri.isDirectory()) throw RequestParser::HttpRequestException("The requested resource is neither a regular file or a directory, or it does not exists at all", 404);

    if (uri.isDirectory()) {
        vector<string> defaultPages = location.getDefaultPages();
        vector<string>::iterator iterator = defaultPages.begin();

        for (; iterator != defaultPages.end(); iterator++) {
            string concatenated = uri.path;

            if (concatenated[concatenated.length() - 1] != '/') concatenated += '/';
            
            concatenated += *(iterator);

            if (uri.isRegularFile(concatenated)) {
                uri.path = concatenated;
                break;
            }
        }

        if (iterator == defaultPages.end() && !location.getDirListings()) throw RequestParser::HttpRequestException("Directory Listing is off and the client is trying to access it", 403);
    }

    if (access(uri.path.c_str(), R_OK) == -1) throw RequestParser::HttpRequestException("No permission to read the file", 403);
    set<string> set = location.getMethods();
    if (set.find("GET") == set.end()) throw RequestParser::HttpRequestException("Method is not allowed for this location", 403);
}

void RequestParser::headerSection( ) {
    
    while (1) {
        size_t pos = stringBuffer.find(CRLF);
        string field = stringBuffer.substr(0, pos);

        if (!field.length()) break;

        pos += 2;
        headers.storeField(field, uri, method);
        stringBuffer = stringBuffer.substr(pos);

        if (stringBuffer.find(CRLF) == string::npos) findCRLF();
    }

    headers.parceFieldValue();

    stringBuffer.erase(0, 2);
}

void RequestParser::requestLine( ) {

    string requestLine = stringBuffer.substr(0, stringBuffer.find(CRLF));

    stringBuffer = stringBuffer.substr(stringBuffer.find(CRLF) + 2);

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
    requestLine();
    
    headerSection();

    if (method == GET) stringBuffer.clear();

    Server server = uri.getHostServer();

    Location location = uri.matchURI(server);

    resolveResource(location);

    if (method == POST) setUploader(server, location);
}
