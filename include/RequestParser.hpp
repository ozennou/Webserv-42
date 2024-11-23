/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 11:26:56 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/23 11:52:25 by mlouazir         ###   ########.fr       */
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

class   RequestParser
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
    RequestParser( );
    RequestParser( const RequestParser& obj );
    RequestParser& operator=( const RequestParser& obj );
    
    RequestParser( int& clientFd, int& socketFd, Socket_map& socket_map );

    void init( void );

    void setBondObject( Bond* bond );
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
    
    bool isRange( void );
    bool isValidRange( void );

    string getRangeFirst( void );
    string getRangeLast( void );
    int getRangeType( void );
    bool getConnectionState( void );

    ~RequestParser( );
};