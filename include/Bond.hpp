/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 14:47:50 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/18 17:54:44 by mlouazir         ###   ########.fr       */
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

    string  buffer;

    ifstream *fileStream;

    RequestParser requestParser;

    ResponseGenerator responseGenerator;
public:
    Bond( int clientFd, int socketFd, Socket_map& socket_map, map<int, string>& statusCodeMap );


    int     getClientFd( void );

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

    ~Bond( );
};
