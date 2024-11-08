/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 20:52:22 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/07 16:01:18 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseGenerator.hpp"

ResponseGenerator::ResponseGenerator( int clientFd ) : clientFd(clientFd), exception(NULL) {
    errorHashMap.insert(make_pair<int, string>(404, " Resource Not Found"));
    errorHashMap.insert(make_pair<int, string>(400, " Bad Request"));
}

void ResponseGenerator::setException( RequestParser::HttpRequestException* exception ) {
    this->exception = new RequestParser::HttpRequestException(*exception);
}

void ResponseGenerator::generateResponse( ) {
    stringstream stream;
    if (exception) {
        stream << "HTTP/1.1 " << exception->statusCode << " " << exception->message << CRLF;
        stream << "Content-Length: 0" << CRLF;
        stream << CRLF;
        responseBuffer += stream.str();
        send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);
        delete exception; exception = NULL;
        return;
    }
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

ResponseGenerator::~ResponseGenerator( ) {
}