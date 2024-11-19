/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 18:44:59 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/19 15:29:23 by mlouazir         ###   ########.fr       */
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

    size_t toRead;

    size_t reading;
    
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

    void generateValidMessage( int statusCode, Uri& uri, string& contentType, string& fileBuffer );

    void completeNormalMessage( );
    void completeRangeMessage( );

    void filterResponseType();

    void NormalGETResponse();
    void RangeGETResponse();

    ~ResponseGenerator();
};