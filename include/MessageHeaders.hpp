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

#define DELI "\"(),/:;<=>?@[\\]{}"

using namespace std;

#include <cctype>

class MessageHeaders
{
    string delimiters;
    string space;

    multimap<string, string> hash;
    
public:
    MessageHeaders();

    void parseFieldName( string& field );

    void parseFieldValue( void );

    void print( );
    ~MessageHeaders();
};