/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:15:28 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 15:26:44 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Bond.hpp"

Bond::Bond( int clientFd ) : requestState(-1), responseState(-1), clientFd(clientFd), requestMachine(clientFd), responseMachine(clientFd) {
}

int Bond::getClientFd( ) {
    return clientFd;
}

void Bond::initParcer( ) {
    try {
        requestMachine.initFromLastPoint();
    }
    catch(const RequestParser::HttpRequestException& e) {
        requestState = BAD;
        responseMachine.setRequestState(requestState, e.statusCode);
        return ;
    }
    requestState = GOOD;
    responseMachine.setRequestState(requestState, 0);
}

void Bond::initBuilder( ) {
    responseMachine.generateResponse();
}

int Bond::getRequestState( ) {
    return requestState;
}

int Bond::getResponseState( ) {
    return responseState;
}

Bond::~Bond( ) {
    
}