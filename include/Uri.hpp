/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:19:39 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/22 15:41:34 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cctype>
#include <map>

#include "Socket_map.hpp"
#include "Server.hpp"
#include "Location.hpp"

using namespace std;

enum {
    ORIGIN,
    ABSOLUTE
};

class Uri
{
public:
    Uri( );
    Uri( const Uri& obj );
    Uri& operator=( const Uri& obj );
    Uri( int& socketFd, Socket_map& socket_map );
    
    ~Uri( );

    int socketFd;

    Socket_map* socket_map;

    int type;

    string subDelimiters;
    string genDelimiters;

    string requestTarget;

    string path;
    string query;

    string host;
    int port;

    void    extractPath( string& requestTarget );
    void    normalizePath( );
    
    Server  getHostServer( );

    Location    matchURI( Server& server );

    bool    isRegularFile( );
    bool    isDirectory( );

    size_t    getResourceSize( );

    void    originForm();
    void    absoluteForm();

    void    extractQuery( size_t& index );
};