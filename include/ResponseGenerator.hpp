/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 18:44:59 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/17 16:45:19 by mlouazir         ###   ########.fr       */
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
    int clientFd;

    size_t size;
    
    ifstream    *ifs;
    
    Bond*       bond;

    RequestParser::HttpRequestException* exception;

    map<int, string>& statusCodeMap;

public:
    
    ResponseGenerator( int clientFd, map<int, string>& statusCodeMap );

    ResponseGenerator& operator=( const ResponseGenerator& obj );

    void setException( RequestParser::HttpRequestException* exception );

    void setBondObject( Bond* bond );

    void setInputStream( ifstream* ifss );

    void generateErrorMessage( );

    void generateValidMessage( Uri& uri, string& contentType, string& fileBuffer );

    void completeMessage( );

    void filterResponseType();

    void GETResponse();

    ~ResponseGenerator();
};