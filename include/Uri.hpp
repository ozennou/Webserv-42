/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:19:39 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/07 17:01:38 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cctype>
#include <map>

#include "Socket_map.hpp"
#include "Server.hpp"

using namespace std;

enum {
    ORIGIN,
    ABSOLUTE
};

class Uri
{
public:

    Socket_map* socket_map;

    int socketFd;
    //Type Of The URI
    int type;

    // SUB_DELIM
    string subDelimiters;

    // GEN_DELIM
    string genDelimiters;

    string requestTarget;

    string path;
    string query; // The Query Can Stay as a string and passed to the CGI

    string host;
    int port;

    void extractPath( string requestTarget );

    void normalizePath( );
    
    Server checkHostInfo( );

    // void decodePercentEncoding();

    void originForm();
    void absoluteForm();

    // bool isUnreserved( int c );
    // bool percentEncoded( string& str, size_t index );

    void extractQuery( size_t index );

    Uri( int socketFd, Socket_map& socket_map );
    ~Uri( );
};