/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 20:52:22 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/03 15:57:18 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseGenerator.hpp"

ResponseGenerator::ResponseGenerator( int clientFd ) : clientFd(clientFd)/*, responseBuffer()*/ {
    errorHashMap.insert(make_pair<int, string>(404, "Resource Not Found"));
}

void ResponseGenerator::setRequestState( int requestState, int statusCode ) {
    this->requestState = requestState;
    if (statusCode > 0)
        this->statusCode = statusCode;
}

void ResponseGenerator::generateResponse( ) {
    stringstream stream;
    if (requestState == BAD) {
        stream << "HTTP/1.1 ";
        stream << statusCode;
        stream << " Bad Request";
        stream << CRLF;
        responseBuffer += stream.str();
        cout << responseBuffer << endl;
        cout << send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0) << endl;
    }
    else if (requestState == GOOD) {
        stream << "HTTP/1.1 ";
        stream << 200;
        stream << " OK";
        stream << CRLF;
        responseBuffer += stream.str();
        cout << responseBuffer << endl;
        send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);
    }
}

ResponseGenerator::~ResponseGenerator( ) {
                                        
}