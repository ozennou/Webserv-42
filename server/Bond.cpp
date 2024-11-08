/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:15:28 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/07 12:21:34 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Bond.hpp"

Bond::Bond( int clientFd, int socketFd, Socket_map& socket_map ) : clientFd(clientFd), requestParser(clientFd, socketFd, socket_map), responseGenerator(clientFd) {
}

int Bond::getClientFd( ) {
    return clientFd;
}

void Bond::initParcer( ) {
    try {
        requestParser.init();
    }
    catch(RequestParser::HttpRequestException& e) {
        if (e.statusCode > 0) {
            logging(e.message, WARNING, NULL, 0);
            responseGenerator.setException(&e);
            return ;
        }
        throw e;
    }
}

void Bond::initBuilder( ) {
    responseGenerator.generateResponse();
}

// void Bond::getPayLoadFromParser( ) {
//     responseGenerator.setPayLoad(requestParser.getPayLoad());
// }

Bond::~Bond( ) {
    
}