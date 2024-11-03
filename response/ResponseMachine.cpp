/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMachine.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 17:00:47 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 15:45:13 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseMachine.hpp"

ResponseMachine::ResponseMachine( int clientFd ) : responseGenerator(clientFd) {

}

void ResponseMachine::setRequestState( int requestState, int statusCode ) {
    responseGenerator.setRequestState(requestState, statusCode);
}

void ResponseMachine::generateResponse( ) {
    responseGenerator.generateResponse();
}

ResponseMachine::~ResponseMachine( ) {
    
}