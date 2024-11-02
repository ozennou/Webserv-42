/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 20:52:22 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 15:30:35 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseGenerator.hpp"

ResponseGenerator::ResponseGenerator( int clientFd ) : clientFd(clientFd) {
    errorHashMap.insert(make_pair<int, string>(404, "Resource Not Found"));
}

void ResponseGenerator::setRequestState( int requestState, int statusCode ) {
    this->requestState = requestState;
    if (statusCode > 0)
        statusCode = statusCode;
}

void ResponseGenerator::generateResponse( ) {
    cout << "Test" << endl;
    if (requestState == BAD) {
        responseBuffer += "HTTP/1.1 ";
        responseBuffer += statusCode;;
        responseBuffer += CRLF;
        cout << responseBuffer << endl;
        send(clientFd, responseBuffer.c_str(), responseBuffer.length(), 0);
    }
}

ResponseGenerator::~ResponseGenerator( ) {
                                        
}