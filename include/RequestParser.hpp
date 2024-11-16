/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 11:26:56 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/15 17:51:57 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define CRLF "\r\n"

#include <exception>
#include <unistd.h>

#include "header.hpp"

#include <sys/stat.h>
#include <fcntl.h>

#include "Uri.hpp"
#include "MessageHeaders.hpp"

class Bond;

using namespace std;

enum {
    GET,
    POST,
    DELETE
};

class RequestParser
{
    Bond*    bond;

    int                 method;
    Uri                 uri;
    MessageHeaders      headers;

    size_t size;

    string  payload;

    int     clientFd;

    void findCRLF( string& stringBuffer);

    void requestLine( string& bufferString );

    void headerSection( string stringBuffer );

    void resolveResource( Location& location );

public:
    RequestParser( int& clientFd, int& socketFd, Socket_map& socket_map, Bond* bond );

    void init( void );

    // Exception Class
    class HttpRequestException : public exception
    {
    public:
        string message;
        int statusCode;

        HttpRequestException( const HttpRequestException& obj ) ;
        HttpRequestException( string message , int statusCode );

        const char* what() const throw();

        ~HttpRequestException() throw();
    };
    //

    int getMethod( void );

    Uri& getUri( void );
    
    ~RequestParser( );
};