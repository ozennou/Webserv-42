/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestParser.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 11:26:56 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/01 21:40:41 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQ_PARCER
#define REQ_PARCER

#define CRLF "\r\n"

#include <exception>
#include <unistd.h>

#include <header.hpp>

#include <sys/stat.h>
#include <fcntl.h>

#include "Uri.hpp"

#include "RequestMessage.hpp"

#include "RequestMachine.hpp"

using namespace std;

class RequestParser
{
    // Tools only
    size_t size;
    // char buf[size];
    //

    // The RequestMessage Object
    RequestMessage* requestMessage;
    //

    // The Socket Fd To read from
    int socketFd;
    //

    void findCRLF( string& stringBuffer);

    void requestLine( string& bufferString );

    void headerSection( string stringBuffer );
public:
    RequestParser( int socketFd, RequestMessage* requestMessage );

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

#endif