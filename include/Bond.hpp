/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 14:47:50 by mlouazir          #+#    #+#             */
/*   Updated: 2024/12/09 11:23:00 by mlouazir         ###   ########.fr       */
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

    RequestParser requestParser;

    ResponseGenerator responseGenerator;
public:
    Bond( );
    Bond( const Bond& obj );
    Bond& operator=( const Bond& obj );

    Bond( int clientFd, int socketFd, Socket_map& socket_map, map<int, string>& statusCodeMap );

    ~Bond( );

    int     getClientFd( void ) const;

    void    initParcer( void );
    void    initResponse( void );

    int     getMethod( void );
    Uri&    getUri( void );

    int     getResponseState( void );
    void     setResponseState( int state );

    int     getPhase( void );
    void     setPhase( int phasee );

    void     setRedirect( pair<int, string> info );

    bool    isCGI( void );
    bool    rangeHeader( void );
    string    getRangeFirst( void );
    string    getRangeLast( void );
    int     getRangeType( void );
    bool     getConnectionState( void );
    int     getUploadState( void );
    void    reset( );
    void    setConnectionState(bool cs);
};