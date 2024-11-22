/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 14:47:50 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/22 17:53:39 by mlouazir         ###   ########.fr       */
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
    int phase;

    int responseState;

    int clientFd;

    bool connectionSate;

    string  buffer;

    ifstream *fileStream;

    RequestParser requestParser;

    ResponseGenerator responseGenerator;
public:
    Bond( );
    Bond( const Bond& obj );
    Bond& operator=( const Bond& obj );

    Bond( int clientFd, int socketFd, Socket_map& socket_map, map<int, string>& statusCodeMap );

    int     getClientFd( void ) const;

    void    initParcer( void );
    void    initResponse( void );

    int     getMethod( void );
    Uri&    getUri( void );
    string* getBuffer( void );

    int     getResponseState( void );
    void     setResponseState( int state );

    int     getPhase( void );
    void     setPhase( int phasee );

    bool    isRange( void );
    string    getRangeFirst( void );
    string    getRangeLast( void );
    int     getRangeType( void );
    bool     getConnectionState( void );

    ~Bond( );
};