/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 11:26:56 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/29 11:41:33 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define CRLF "\r\n"

#include <exception>
#include <unistd.h>

#include <header.hpp>

#include <sys/stat.h>
#include <fcntl.h>

#include "Request.hpp"
#include "Uri.hpp"

class Request;

using namespace std;

class RequestParser
{
    // Tools only
    size_t size;
    // char buf[size];
    //

    // The Request Object
    Request request;
    //

    // The Socket Fd To read from
    int socketFd;
    //

    void findCRLF( string& stringBuffer);

    void requestLine( string& bufferString );

    void headerSection( string stringBuffer );
public:
    RequestParser( int socketFd );

    void fillRequestObject();
    
    // Exception Class
    class HttpRequestException : public exception
    {

        // could need a code here to identify the cause of the exception
    public:
        string message;
        int statusCode;
        HttpRequestException( string message , int statusCode );
        const char* what() const throw();
        ~HttpRequestException() throw();
    };
    //

    ~RequestParser( );
};