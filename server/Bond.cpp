/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:15:28 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/22 20:55:00 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Bond.hpp"

string stolower( string s ) {
    for (size_t i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) s[i] = tolower(s[i]);
    }
    return s;
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

Bond::Bond( int clientFd, int socketFd, Socket_map& socket_map, map<int, string>& statusCodeMap ) : phase(REQUEST_READY), responseState(CLOSED), clientFd(clientFd), fileStream(NULL), requestParser(clientFd, socketFd, socket_map, this), responseGenerator(clientFd, statusCodeMap) {
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
        this->buffer = obj.buffer;
        this->fileStream = obj.fileStream;
        this->requestParser = obj.requestParser;
        this->responseGenerator = obj.responseGenerator;
    }
    return *this;
}

Bond::Bond( int clientFd, int socketFd, Socket_map& socket_map, map<int, string>& statusCodeMap ) : phase(REQUEST_READY), responseState(CLOSED), clientFd(clientFd), connectionSate(true), fileStream(NULL), requestParser(clientFd, socketFd, socket_map, this), responseGenerator(clientFd, statusCodeMap) {
}


int Bond::getClientFd( ) const {
    return clientFd;
}

void Bond::initParcer( ) {
    try {
        setPhase(RESPONSE_READY);
        requestParser.init();
    }
    catch(RequestParser::HttpRequestException& e) {
        if (e.statusCode > 0) {
            logging(e.message, WARNING, NULL, 0);
            responseGenerator.setException(&e);
            return ;
        }
        throw e;
    }
}

void Bond::initResponse( ) {
    if (phase == REQUEST_READY)  return ;

    if (!fileStream) {
        fileStream = new ifstream();
        responseGenerator.setInputStream(fileStream);
    }

    responseGenerator.setBondObject(this);
    cout << "res="<< getClientFd() << endl;
    responseGenerator.filterResponseType();
}

int Bond::getMethod( ) {
    return GET;
}

Uri& Bond::getUri( ) {
    return requestParser.getUri();
}

string* Bond::getBuffer( ) {
    return &buffer;
}

int Bond::getResponseState( ) {
    return responseState;
}

void Bond::setResponseState( int statee ) {
    this->responseState = statee;
}

int Bond::getPhase( ) {
    return phase;
}

void Bond::setPhase( int statee ) {
    this->phase = statee;
}

Bond::~Bond( ) {
    if (!fileStream) delete fileStream; fileStream = NULL;
}

void Bond::methodInfosGET( void ) {
    
}
