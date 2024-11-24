/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageHeaders.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 17:42:56 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/30 17:12:08 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <iostream>

#include "Uri.hpp"

#define DELI "\"(),/:;<=>?@[\\]{}"

using namespace std;

#include <cctype>

class MessageHeaders
{
    int rangeType;

    string first;
    string last;

    string delimiters;
    string space;

    multimap<string, string> headers;
    
public:
    MessageHeaders( );
    MessageHeaders( const MessageHeaders& obj );
    MessageHeaders& operator=( const MessageHeaders& obj );

    ~MessageHeaders( );
    
    void storeRange( string& fieldValue );
    bool isValidRange( Uri& uri );
    int getRangeType( );
    string getRangeFirst( );
    string getRangeLast( );

    void storeHost( string& fieldValue, Uri& uri );

    void storeField( string& field, Uri& uri, int method );
    void parceFieldValue( void );
    bool findField( string fieldName );

    bool connectionState( );

    map<string, string>::iterator findContentHeaders( );
};