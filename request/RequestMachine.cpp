/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMachine.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:47:46 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/01 15:42:02 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/RequestMachine.hpp"

RequestMachine::RequestMachine( int clientFd ) : requestMessage(new RequestMessage()), requestParser(clientFd, requestMessage) {

}

void RequestMachine::initFromLastPoint( void ) {
    requestParser.init();
}

RequestMachine::~RequestMachine( ) {
    
}