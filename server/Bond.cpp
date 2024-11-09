/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:15:28 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/09 14:11:51 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Bond.hpp"

string stolower( string s ) {
    for (size_t i = 0; i < s.length(); i++) {
        if (isalpha(s[i])) s[i] = tolower(s[i]);
    }
    return s;
}

bool isUnreserved( int c ) {
    return isdigit(c) || isalpha(c) || c == '-' || c == '.' || c == '_' || c == '~';
}

bool percentEncoded( string& str, size_t index ) {
    if (str[index] != '%') return false;
    
    if ((index + 2 < str.length()) && (isxdigit(str[index + 1]) && isxdigit(str[index + 2]))) {
        unsigned int x;
        stringstream stream;
        string percentHex = str.substr(index + 1, 3);
        
        stream << hex << percentHex; // Should Check More What Is Hex HERE
        stream >> x;

        str.erase(index, 3);
        str.insert(index, 1, x);
        return true;
    }
    return false;
}

// ------------------------------------------------------------------------------------------------------ //

Bond::Bond( int clientFd, int socketFd, Socket_map& socket_map ) : clientFd(clientFd), requestParser(clientFd, socketFd, socket_map), responseGenerator(clientFd) {
}

int Bond::getClientFd( ) {
    return clientFd;
}

void Bond::initParcer( ) {
    try {
        requestParser.init();
    }
    catch(RequestParser::HttpRequestException& e) {
        if (e.statusCode > 0) {
            logging(e.message, WARNING, NULL, 0);
            return ;
        }
        responseGenerator.setException(&e);
        throw e;
    }
}

void Bond::initBuilder( ) {
    responseGenerator.generateResponse();
}

// void Bond::getPayLoadFromParser( ) {
//     responseGenerator.setPayLoad(requestParser.getPayLoad());
// }

Bond::~Bond( ) {
    
}