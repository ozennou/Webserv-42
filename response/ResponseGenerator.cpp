/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 20:52:22 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/12 16:59:05 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseGenerator.hpp"

ResponseGenerator::ResponseGenerator( int clientFd, Bond* bond ) : bond(bond), exception(NULL), clientFd(clientFd) {
    errorHashMap.insert(make_pair<int, string>(505, "HTTP Version Not Supported"));
    errorHashMap.insert(make_pair<int, string>(501, "Not Implemented"));
    errorHashMap.insert(make_pair<int, string>(405, "Method Not Allowed"));
    errorHashMap.insert(make_pair<int, string>(404, "Not Found"));
    errorHashMap.insert(make_pair<int, string>(403, "Forbidden"));
    errorHashMap.insert(make_pair<int, string>(400, "Bad Request"));
}

void ResponseGenerator::setException( RequestParser::HttpRequestException* exc ) {
    this->exception = new RequestParser::HttpRequestException(*exc);
}


void ResponseGenerator::getRequestUri( void ) {
    bond->getUri();
}


void ResponseGenerator::generateResponse( ) {
    stringstream stream;
    if (exception) {
        cout << "Exception" << endl;
        stream << "HTTP/1.1 " << exception->statusCode << " " << errorHashMap.find(exception->statusCode)->second << CRLF;
        stream << "Content-Length: 0" << CRLF;
        stream << CRLF;
        responseBuffer += stream.str();
        send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);
        delete exception; exception = NULL;
        return;
    } else {
        stream << "HTTP/1.1 " << 200 << " OK" << CRLF;
        stream << "Date: Thu, 16 Nov 2017 16:40:10 GMT" << endl;
        stream << "Content-Length: 1" << CRLF;
        stream << CRLF;
        stream << "H";
        stream << "";
        responseBuffer += stream.str();
        send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);
        responseBuffer = "";
    }
}

ResponseGenerator::~ResponseGenerator( ) {
}

void ResponseGenerator::setPath( string path ) {
    cout << "******------>"<< path << endl;
}