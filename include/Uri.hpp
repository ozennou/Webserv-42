/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:19:39 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/25 15:25:25 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <cctype>
#include <map>

#include "RequestParser.hpp"

#define GEN_DELIM ":/?#[]@"
#define SUB_DELIM "!$&'()*+,;="

using namespace std;

enum {
    ORIGIN,
    ABSOLUTE
};

class Uri
{
    //Type Of The URI
    int type;

    // SUB_DELIM
    string subDelimiters;

    // GEN_DELIM
    string genDelimiters;

    string& requestTarget;

    string path;
    string query; // The Query Can Stay as a string and passed to the CGI

    string host;
    string port;

    void extractPath();

    // void decodePercentEncoding();

    void originForm();
    void absoluteForm();

    bool isUnreserved( int c );
    bool percentEncoded( string& str, size_t index );

    void extractQuery( size_t index );
public:
    Uri( string& requestTarget );
    ~Uri( );
};