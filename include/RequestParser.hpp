/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 11:26:56 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/25 21:37:49 by mlouazir         ###   ########.fr       */
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


    void requestLine( string bufferString );

    void headerSection( char *buf );
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