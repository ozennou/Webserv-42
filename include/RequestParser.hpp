/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 11:26:56 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 15:17:19 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define CRLF "\r\n"

#include <exception>
#include <unistd.h>

#include <header.hpp>

#include <sys/stat.h>
#include <fcntl.h>

#include "Uri.hpp"
#include "MessageHeaders.hpp"

using namespace std;

enum {
    GET,
    POST,
    DELETE
};

class RequestParser
{
    // The RequestMessage Object
    int         methode;
    Uri*        uri;
    MessageHeaders*    headers;
    //

    // Tools only
    size_t size;
    // char buf[size];
    //

    // The Socket Fd To read from
    int socketFd;
    //

    void findCRLF( string& stringBuffer);

    void requestLine( string& bufferString );

    void headerSection( string stringBuffer );
public:
    RequestParser( int socketFd );

    void init( void );
    
    // Exception Class
    class HttpRequestException : public exception
    {

        // could need a code here to identify the cause of the exception
    public:
        string message;
        int statusCode;
        int state;
        HttpRequestException( string message , int statusCode, int state );
        const char* what() const throw();
        ~HttpRequestException() throw();
    };
    //

    ~RequestParser( );
};