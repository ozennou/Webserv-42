/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMachine.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:47:46 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/01 21:16:54 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RequestMachine.hpp"

RequestMachine::RequestMachine( int clientFd ) : requestMessage(new RequestMessage()), requestParser(clientFd, requestMessage) {

}

void RequestMachine::initFromLastPoint( void ) {
    try {
        requestParser.init();
    }
    catch(const RequestParser::HttpRequestException& r) {
        // r.statusCode
    }
    
}

RequestMachine::~RequestMachine( ) {
    
}