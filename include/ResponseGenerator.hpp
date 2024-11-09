/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 18:44:59 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/08 14:48:52 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <header.hpp>

#include "MessageHeaders.hpp"
#include "RequestParser.hpp"

class RequestParser;

class ResponseGenerator
{
    // Response Component
    // int statusCode;

    string reasonPhrase;

    // MessageHeaders* headers;
    //

    RequestParser::HttpRequestException* exception;

    // // Buffer
    string  responseBuffer;

    string payload;

    map<int, string> errorHashMap;

    //
    int clientFd;
public:
    
    ResponseGenerator( int clientFd );

    void setException( RequestParser::HttpRequestException* exception );

    void generateResponse();

    ~ResponseGenerator();
};