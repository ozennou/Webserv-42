/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 20:52:22 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 14:54:25 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseGenerator.hpp"

ResponseGenerator::ResponseGenerator( ResponseMessage* responseMessage ) : responseMessage(responseMessage) {
    errorHashMap.insert(make_pair<int, string>(404, "Resource Not Found"));
}

void ResponseGenerator::setRequestState( int requestState, int statusCode ) {
    this->requestState = requestState;
    if (statusCode > 0)
        statusCode = statusCode;
}

void ResponseGenerator::generateResponse( ) {
    if (requestState == BAD) {

    }
}

ResponseGenerator::~ResponseGenerator( ) {
                                        
}