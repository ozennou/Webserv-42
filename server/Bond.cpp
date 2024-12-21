/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:15:28 by mlouazir          #+#    #+#             */
/*   Updated: 2024/12/21 10:35:35 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Bond.hpp"

void stolower( string& s ) {
    for (size_t i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) s[i] = tolower(s[i]);
    }
}

bool isUnreserved( int c ) {
    return isdigit(c) || isalpha(c) || c == '-' || c == '.' || c == '_' || c == '~';
}

bool percentEncoded( string& str, size_t index ) {
    if (str[index] != '%') return false;
    
    if ((index + 2 < str.length()) && (isxdigit(str[index + 1]) && isxdigit(str[index + 2]))) {
        unsigned int x;
        stringstream stream;
        string percentHex = str.substr(index + 1, 3);
        
        stream << hex << percentHex; // Should Check More What Is Hex HERE
        stream >> x;

        str.erase(index, 3);
        str.insert(index, 1, x);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------ //

Bond::Bond( ) {
}

Bond::Bond( const Bond& obj ) {
    *this = obj;
}

Bond& Bond::operator=( const Bond& obj ) {
    if (this != &obj) {
        this->phase = obj.phase;
        this->responseState = obj.responseState;
        this->clientFd = obj.clientFd;
        this->connectionSate = obj.connectionSate;
        this->requestParser = obj.requestParser;
        this->responseGenerator = obj.responseGenerator;
        this->sa = obj.sa;
        this->pipeFd = obj.pipeFd;
        this->p = obj.p;
        this->cgiPhase = obj.cgiPhase;
        this->isCgi = obj.isCgi;
        this->cgiTimeout = 0;
        this->responseGenerator.setBondObject(this);
        this->requestParser.setBondObject(this);
    }
    return *this;
}

Bond::Bond( int clientFd, int socketFd, Socket_map& socket_map, map<int, string>& statusCodeMap, sockaddr_storage sa) : phase(REQUEST_READY), responseState(CLOSED), clientFd(clientFd), connectionSate(true), requestParser(clientFd, socketFd, socket_map), sa(sa), responseGenerator(clientFd, statusCodeMap), cgiPhase(false), isCgi(false) {
    pipeFd = -1;
    p = -1;
}

void Bond::initParcer( ) {
    try {
        if (requestParser.getMethod() == POST && requestParser.getUploadState() == UPLOADING) {
            requestParser.upload();
            return;
        }
        
        if (phase == RESPONSE_READY) {
            int i;
            char buf[200000];

            i = recv(clientFd, buf, 200000, 0);

            if (!i) throw RequestParser::HttpRequestException("Connection Ended", 0);
            if (i == -1) throw RequestParser::HttpRequestException("Nothing Yet", -1);

            return ;
        }

        int i;
        char buf[5621];

        i = recv(clientFd, buf, 5620, 0);

        if (!i) throw RequestParser::HttpRequestException("Connection Ended", 0);
        if (i == -1) throw RequestParser::HttpRequestException("Nothing Yet", -1);

        string& stringBuffer = requestParser.getStringBuffer();

        stringBuffer.append(buf, i);

        if (stringBuffer.find("\r\n\r\n") == string::npos) {
            return ;
        }

        setPhase(RESPONSE_READY);
        requestParser.init();
        connectionSate = requestParser.getConnectionState();
    }
    catch(RequestParser::HttpRequestException& e) {
        if (e.statusCode > 0) {
            responseGenerator.setException(&e);
            connectionSate = requestParser.getConnectionState();
            requestParser.reset();
            return ;
        }
        throw e;
    }
}

void Bond::initResponse( ) {
    if (phase != RESPONSE_READY || requestParser.getUploadState() != UPLOADED)  return;

    if (isCgi)
        responseGenerator.CgiWait();
    else
        responseGenerator.filterResponseType();
}

int Bond::getClientFd( ) const {
    return clientFd;
}

int Bond::getMethod( ) {
    return requestParser.getMethod();
}

Uri& Bond::getUri( ) {
    return requestParser.getUri();
}

int Bond::getResponseState( ) {
    return responseState;
}

void Bond::setResponseState( int statee ) {
    this->responseState = statee;
}

void    Bond::setConnectionState(bool cs) {
    connectionSate = cs;
}

int Bond::getPhase( ) {
    return phase;
}

unsigned int Bond::getCgiTimeout( void ) {
    return cgiTimeout;
}

Uploader& Bond::getUploader( void ) {
    return requestParser.getUploader();
}

void Bond::setPhase( int statee ) {
    this->phase = statee;
}

void Bond::setRedirect( pair<int, string> info ) {
    responseGenerator.setRedirect(info);
}

void Bond::setErrorPages( map<int, string> errorPages ) {
    responseGenerator.setErrorPages(errorPages);
}

void Bond::setCgiTimeout( unsigned int cgiTimeoutt ) {
    this->cgiTimeout = cgiTimeoutt;
}

string  Bond::getRangeFirst( void ) {
    return requestParser.getRangeFirst();
}

string  Bond::getRangeLast( void ) {
    return requestParser.getRangeLast();
}

void Bond::setCgiPhase(bool _phase) {
    cgiPhase = _phase;
}

bool Bond::getCgiPhase() {
    return cgiPhase;
}

int  Bond::getRangeType( void ) {
    return requestParser.getRangeType();
}

bool  Bond::getConnectionState( void ) {
    return connectionSate;
}

int  Bond::getUploadState( void ) {
    return requestParser.getUploadState();
}

bool  Bond::isCGI( void ) {
    return requestParser.isCGI();
}

bool Bond::rangeHeader( void ) {
    if (requestParser.isRange() && !requestParser.isValidRange()) {
        RequestParser::HttpRequestException e("Range Condition Not Good", 416);
        responseGenerator.setException(&e);
    }
    return requestParser.isRange() && requestParser.isValidRange();
}

map<string, string>& Bond::getHeaders( void ) {
    return requestParser.getHeaders();
}

void  Bond::reset( void ) {
    // TODO: Keep an eye here
    phase = REQUEST_READY;
    responseState = CLOSED;
    requestParser.reset();
    responseGenerator.reset();
}

string  Bond::getRemoteHost() const {
    return getRemoteAddr();
}
string  Bond::getRemoteAddr() const {
    char ip_str[INET6_ADDRSTRLEN];

    struct sockaddr_in *test = (struct sockaddr_in *)&sa;
    if(!inet_ntop(AF_INET, &(test->sin_addr), ip_str, sizeof(ip_str)))
        return "";
    return ip_str;
}

Bond::~Bond( ) {
    close(pipeFd);
}

pair<int, pid_t>    Bond::getCgiInfos() const {
    return pair<int, pid_t>(pipeFd, p);
}

void                Bond::setCgiInfos(int fd, pid_t _p) {
    pipeFd = fd;
    p = _p;
}