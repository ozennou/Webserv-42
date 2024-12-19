/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 14:47:50 by mlouazir          #+#    #+#             */
/*   Updated: 2024/12/09 15:24:37 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "header.hpp"

#include <iostream>

#include "RequestParser.hpp"

#include "ResponseGenerator.hpp"


class Bond
{
private:
    int phase; // The Phase Of The Flow Of The Connection, RESPONSE_READY or REQUEST_READY.

    int responseState; // The State Of The Response, Is The Full Request Is Sent -> CLOSED, or We Still Have To Send The Buffer -> OPEN.

    int clientFd;

    bool connectionSate; // keep-alive = true, close = false

    int  pipeFd;

    pid_t p;

    RequestParser requestParser;

    sockaddr_storage sa;

    ResponseGenerator responseGenerator;
    
    bool cgiPhase;

    unsigned int cgiTimeout;

public:

    bool isCgi;

    Bond( );
    Bond( const Bond& obj );
    Bond& operator=( const Bond& obj );

    Bond( int clientFd, int socketFd, Socket_map& socket_map, map<int, string>& statusCodeMap, sockaddr_storage sa);

    ~Bond( );

    int     getClientFd( void ) const;
    int     getMethod( void );
    Uri&    getUri( void );
    int     getResponseState( void );
    int     getPhase( void );
    string  getRangeFirst( void );
    string  getRangeLast( void );
    int     getRangeType( void );
    bool    getConnectionState( void );
    int     getUploadState( void );
    Uploader&     getUploader( void );
    unsigned int    getCgiTimeout( void );
    map<string, string>& getHeaders( void );

    void     setPhase( int phasee );
    void     setResponseState( int state );
    void     setRedirect( pair<int, string> info );
    void     setConnectionState( bool cs );
    void     setErrorPages( map<int, string> errorPages );
    void     setCgiTimeout( unsigned int cgiTimeout );

    void    initParcer( void );
    void    initResponse( );

    bool    isCGI( void );

    bool    rangeHeader( void );

    void    reset( );

    string  getRemoteAddr() const;
    string  getRemoteHost() const;

    pair<int, pid_t>    getCgiInfos() const;
    void                setCgiInfos(int fd, pid_t _p);

    void                setCgiPhase(bool _phase);
    bool                getCgiPhase();
};