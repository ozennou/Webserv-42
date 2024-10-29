/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 17:42:56 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/28 20:33:04 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <iostream>

#define DELI "\"(),/:;<=>?@[\\]{}"

using namespace std;

class Headers
{
    string delimiters;
    string space;

    map<string, string> mapp;
    
public:
    Headers();

    void parseField( string& field );

    void print( );
    ~Headers();
};