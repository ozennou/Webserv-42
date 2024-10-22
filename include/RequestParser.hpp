/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 11:26:56 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/21 22:22:02 by mlouazir         ###   ########.fr       */
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


    void requestLine( string& bufferString, bool isEmpty );

    void headerSection( char *buf, bool isEmpty );
public:
    RequestParser( int socketFd );

    void fillRequestObject();
    
    // Exception Class
    class HttpRequestException : public exception
    {
        string message;

        // could need a code here to identify the cause of the exception
    public:
        int statusCode;
        HttpRequestException( string message , int statusCode );
        const char* what() const throw();
        ~HttpRequestException() throw();
    };
    //

    ~RequestParser( );
};