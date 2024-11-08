/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 11:26:56 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/05 11:45:04 by mlouazir         ###   ########.fr       */
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
    int                 methode;
    Uri                 uri;
    MessageHeaders      headers;
    //

    // Tools only
    size_t size;
    // char buf[size];
    //

    string  payload;
    // The Socket Fd To read from
    int clientFd;
    //

    void findCRLF( string& stringBuffer);

    void requestLine( string& bufferString );

    void headerSection( string stringBuffer );

    void checkTargetUri( Server& server );
public:
    RequestParser( int clientFd, int socketFd, Socket_map& socket_map );

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

    ~RequestParser( );
};