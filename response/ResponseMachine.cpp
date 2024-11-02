/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMachine.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 17:00:47 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 14:51:52 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseMachine.hpp"

ResponseMachine::ResponseMachine( ) : responseMessage(new ResponseMessage()), responseGenerator(responseMessage) {

}

void ResponseMachine::setRequestState( int requestState, int statusCode ) {
    responseGenerator.setRequestState(requestState, statusCode);
}

void ResponseMachine::generateResponse( ) {
    responseGenerator;
}

ResponseMachine::~ResponseMachine( ) {
    
}