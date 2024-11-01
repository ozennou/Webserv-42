/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:15:28 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/01 15:36:11 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Bond.hpp"

Bond::Bond( int clientFd ) : clientFd(clientFd), requestMachine(clientFd) {

}

int Bond::getClientFd( ) {
    return clientFd;
}

void Bond::initParcing( ) {
    requestMachine.initFromLastPoint();
}


Bond::~Bond( ) {
    
}