/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:15:28 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/01 21:01:07 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Bond.hpp"

Bond::Bond( int clientFd ) : state(-1), clientFd(clientFd), requestMachine(clientFd) {
}

int Bond::getClientFd( ) {
    return clientFd;
}

void Bond::initParcing( ) {
    try {
        requestMachine.initFromLastPoint();
    }
    catch(const RequestParser::HttpRequestException& e) {
        // state = BAD;
    }
}


Bond::~Bond( ) {
    
}