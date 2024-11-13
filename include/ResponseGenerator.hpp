/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 18:44:59 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/13 17:53:57 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include "header.hpp"

#include "MessageHeaders.hpp"
#include "RequestParser.hpp"

class RequestParser;

class Bond;

class ResponseGenerator
{
    Bond*    bond;
    string reasonPhrase;

    // MessageHeaders* headers;
    //
    RequestParser::HttpRequestException* exception;

    string  responseBuffer;

    string payload;

    map<int, string> errorHashMap;

    int clientFd;
public:
    
    ResponseGenerator( int clientFd );

    void setException( RequestParser::HttpRequestException* exception );

    void setBondObject( Bond* bond );

    void generateMessageError( );

    void generateMessageValid( Uri& uri, string& contentType, string& fileBuffer );

    void filterResponseType();

    void GETResponse();

    ~ResponseGenerator();
};