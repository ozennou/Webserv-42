/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 18:44:59 by mlouazir          #+#    #+#             */
/*   Updated: 2024/12/10 09:02:34 by mlouazir         ###   ########.fr       */
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
    
    ifstream    ifs;

    Bond*       bond;

    bool                isRedirect;
    pair<int, string>   redirectPair;

    RequestParser::HttpRequestException* exception;
    map<int, string>* statusCodeMap;
    map<int, string> errorPages;

public:
    
    ResponseGenerator( );
    ResponseGenerator( const ResponseGenerator& obj );
    ResponseGenerator& operator=( const ResponseGenerator& obj );

    ResponseGenerator( int clientFd, map<int, string>& statusCodeMap );

    ~ResponseGenerator();
    
    void setException( RequestParser::HttpRequestException* exception );
    void setBondObject( Bond* bond );
    void setRedirect( pair<int, string> info );
    void setErrorPages( map<int, string> errorPages );

    void generateErrorMessage( );

    void generateValidMessage( int statusCode, Uri& uri, string& contentType, string& fileBuffer, size_t rangeFirst, size_t rangeLast);

    void completeNormalMessage( );
    void completeRangeMessage( );

    void filterResponseType();

    void CGI();

    void RedirectionResponse();
    void NormalGETResponse();
    void RangeGETResponse();
    void POSTResponse();
    void DELETEResponse();
    
    string dirlisting();
    void directoryResponse();

    void reset();

    char** cgiEnvs();

    void CgiWait(void) ;
};