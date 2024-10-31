/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 21:10:13 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/30 21:37:49 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Response.hpp"

Response::Response( ) : headers(NULL) {

}

void Response::insertHeaders( ) {
    if (!headers) return ;
}

void Response::generateResponse( ) {
    string statusLine = "HTTP/1.1 ";
    statusLine += statusCode;
    statusLine += " ";
    statusLine += reasonPhrase;
    statusLine += CRLF;
}

Response::~Response( ) {
    
}