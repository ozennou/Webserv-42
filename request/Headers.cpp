/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Headers.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 18:18:49 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/28 21:23:24 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Headers.hpp"
#include "../include/RequestParser.hpp"

Headers::Headers( ) : delimiters(DELI), space(" \t") {

}

void Headers::parseField( string& field ) {
    size_t colonIndex = field.find(':');

    if (colonIndex == string::npos) throw RequestParser::HttpRequestException("no colon ':' found in the field", 400);
    if (!colonIndex) throw RequestParser::HttpRequestException("no field name found", 400);

    size_t i = 0;
    for (; i < colonIndex; i++) {
        if (delimiters.find(field[i]) != string::npos \
        || space.find(field[i]) != string::npos)
            throw RequestParser::HttpRequestException("Invalid Character Found in the field-name", 400);
    }
    
    mapp.insert(make_pair<string, string>(field.substr(0, colonIndex), \
                                        field.substr( field.find_first_not_of(space, colonIndex + 1), \
                                                    field.find_last_not_of(space) - field.find_first_not_of(space, colonIndex + 1) \
                                                    )));
}

void Headers::print( ) {
    for (map<string, string>::iterator i = mapp.begin(); i != mapp.end(); i++) {
        cout << i->first << i->second << endl;
    }
}

Headers::~Headers( ) {
    
}