/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 18:44:59 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/22 15:39:29 by mlouazir         ###   ########.fr       */
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

    map<int, string>* statusCodeMap;

public:
    
    ResponseGenerator( );
    ResponseGenerator( const ResponseGenerator& obj );
    ResponseGenerator& operator=( const ResponseGenerator& obj );

    ResponseGenerator( int clientFd, map<int, string>& statusCodeMap );

    void setException( RequestParser::HttpRequestException* exception );
    void setBondObject( Bond* bond );
    void setInputStream( ifstream* ifss );

    void generateErrorMessage( );

    void generateValidMessage( int statusCode, Uri& uri, string& contentType, string& fileBuffer, size_t rangeFirst, size_t rangeLast);

    void completeNormalMessage( );
    void completeRangeMessage( );

    void filterResponseType();

    void NormalGETResponse();
    void RangeGETResponse();

    ~ResponseGenerator();
};