/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:34:55 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/25 22:07:10 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Uri.hpp"

Uri::Uri( string& requestTarget ) : subDelimiters(SUB_DELIM), genDelimiters(GEN_DELIM), requestTarget(requestTarget) {
    extractPath();
}

Uri::~Uri( ) {

}

bool Uri::isUnreserved( int c ) {
    return isdigit(c) || isalpha(c) || c == '-' || c == '.' || c == '_' || c == '~';
}

bool Uri::percentEncoded( string& str, size_t index ) {
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

void Uri::extractQuery( size_t index ) {
    query = requestTarget.substr(index + 1, requestTarget.length() - index + 1);
}

void Uri::extractPath( ) {
    if (requestTarget[0] == '/') {
        originForm();
        type = ORIGIN;
    }
    else {
        absoluteForm();
        type = ABSOLUTE;
        if (!port.length()) port = "80";
        if (!path.length()) path = "/";
        cout << "the host is= "<< host << endl;
        cout << "the port is= "<< port << endl;
    }
    cout << "the path is= "<< path << endl;
    cout << "the query is= "<< query << endl;
}

void Uri::originForm( ) {
    size_t i = 1;
    for (; i < requestTarget.length(); i++) {
        if ((!isUnreserved(requestTarget[i]) && requestTarget[i] != '@' && requestTarget[i] != ':') \
        && (requestTarget[i] == '%' && !percentEncoded(requestTarget, i)) \
        && subDelimiters.find(requestTarget[i]) == string::npos)
            throw RequestParser::HttpRequestException("Invalid Character found in path in Origin Form", 400);
        if (requestTarget[i] == '?')
            break;
    }
    path = requestTarget.substr(0, i);
    if (i != requestTarget.length()) extractQuery(i);
}

void Uri::absoluteForm( ) {
    size_t colonIndex = requestTarget.find(':');

    if (colonIndex == string::npos) throw RequestParser::HttpRequestException("':' not found in absolute-fome URI", 400);
    if (colonIndex != 4 || requestTarget.compare(0, 4, "http")) throw RequestParser::HttpRequestException("Invalid scheme received for the absolut-form", 400);

    size_t i = 4;

    if ((i + 2 >= requestTarget.length()) \
    || requestTarget[i + 1] != '/' \
    || requestTarget[i + 2] != '/')
        throw RequestParser::HttpRequestException("No // found after the scheme", 400);
    
    i += 3;

    for (; i < requestTarget.length(); i++) {
        if (genDelimiters.find(requestTarget[i]) != string::npos)
            break;
        if (!isUnreserved(requestTarget[i]) \
        && (requestTarget[i] == '%' && !percentEncoded(requestTarget, i)) \
        && subDelimiters.find(requestTarget[i]) == string::npos)
            throw RequestParser::HttpRequestException("Invalid Character found in path in Origin Form", 400);
    }

    host = requestTarget.substr(7, (i -  (7)));

    if (!host.length()) throw RequestParser::HttpRequestException("No Host Found in absolut-form", 400);

    if (i == requestTarget.length()) return ;

    if ((requestTarget[i] != '/') && (requestTarget[i] != '?') && (requestTarget[i] != ':'))
        throw RequestParser::HttpRequestException("Invalid delimiter after host", 400);
    
    if (requestTarget[i] == ':') {
        size_t j = i + 1;
        for (; j < requestTarget.length(); j++) {
            if (genDelimiters.find(requestTarget[j]) != string::npos)
                break;
            if (!isdigit(requestTarget[j]))
                throw RequestParser::HttpRequestException("Invalid Character In the Port Number", 400);
        }

        port = requestTarget.substr(i + 1, (j - i + 1));

        if (!port.length()) throw RequestParser::HttpRequestException("No Port Found in absolut-form after ':'", 400);

        i = j;
        if (i == requestTarget.length()) return;

        if (requestTarget[i] != '/' && requestTarget[i] != '?')
            throw RequestParser::HttpRequestException("Invalid delimiter after port", 400);

    }

    if (requestTarget[i] == '/') {
        size_t j = i + 1;
        for (; j < requestTarget.length(); j++) {
            if (genDelimiters.find(requestTarget[j]) != string::npos)
                break;
            if ((!isUnreserved(requestTarget[j]) && requestTarget[j] != '@' && requestTarget[j] != ':') \
            && (requestTarget[j] == '%' && !percentEncoded(requestTarget, j)) \
            && subDelimiters.find(requestTarget[j]) == string::npos)
                throw RequestParser::HttpRequestException("Invalid character found in path", 400);
        }

        path = requestTarget.substr(i ,j - i);

        i = j;
        if (i == requestTarget.length()) return;

        if (requestTarget[i] != '?') throw RequestParser::HttpRequestException("Invalid character after path", 400);
    }

    if (requestTarget[i] == '?') extractQuery(i);
}
