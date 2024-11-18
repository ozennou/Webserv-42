/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   MessageHeaders.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/28 18:18:49 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/30 17:12:19 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/MessageHeaders.hpp"
#include "../include/RequestParser.hpp"

MessageHeaders::MessageHeaders( ) : delimiters(DELI), space(" \t") {
}

MessageHeaders::~MessageHeaders( ) {
}


map<string, string>::iterator MessageHeaders::findContentHeaders( ) {
    map<string, string>::iterator it = hash.find("Transfer-Encoding");
    if (it == hash.end()) it = hash.find("Content-Length");
    if (it == hash.end()) throw RequestParser::HttpRequestException("No Content Related Headers Found", 400);
    return it;
}

void MessageHeaders::parceFieldValue( ) {
    for (multimap<string, string>::iterator it = hash.begin(); it != hash.end(); it++) {
        for (size_t i = 0; i < it->second.length(); i++) {
            int character = it->second[i];
            if ((character < 0 || character > 255) \
            || (!isprint(character) && space.find(character) == string::npos))
                throw RequestParser::HttpRequestException("Invalid Character Found in the field-value", 400);
        }
    }
}

void MessageHeaders::storeHost( string& fieldValue, Uri& uri ) {
    string subDelimiters = SUB_DELIM;

    for (size_t j = 0; j < fieldValue.length(); j++) {
        if (fieldValue[j] != ':')
            break;
        if (!isUnreserved(fieldValue[j]) \
        && !percentEncoded(fieldValue, j) \
        && subDelimiters.find(fieldValue[j]) == string::npos)
            throw RequestParser::HttpRequestException("Invalid Char Found In Host field-value", 400);
    }
    if (fieldValue.find(':') == string::npos) uri.host = fieldValue;
    else uri.host = fieldValue.substr(0, fieldValue.find(':'));

    // If It Contains A Port Number
    if (fieldValue.find(':') != string::npos \
    && fieldValue.find(':') != fieldValue.length() - 1) {
        stringstream ss;
        // for (size_t i = fieldValue.find(':') + 1; i < fieldValue.length(); i++) {
        ss << fieldValue.substr(fieldValue.find(':') + 1, fieldValue.length() - (fieldValue.find(':') + 1));
        // }
        ss >> uri.port;
        if (ss.fail() || uri.port < 0 || uri.port > 65535) throw RequestParser::HttpRequestException("Invalid Port Number", 400);
    }
}

void MessageHeaders::storeField( string& field, Uri& uri ) {
    size_t colonIndex = field.find(':');

    if (colonIndex == string::npos) throw RequestParser::HttpRequestException("no colon ':' found in the field", 400);
    if (!colonIndex) throw RequestParser::HttpRequestException("no field name found", 400);

    size_t i = 0;
    for (; i < colonIndex; i++) {
        if (delimiters.find(field[i]) != string::npos \
        || space.find(field[i]) != string::npos)
            throw RequestParser::HttpRequestException("Invalid Character Found in the field-name", 400);
    }

    string fieldName = field.substr(0, colonIndex); stolower(fieldName);
    string fieldValue;
    if (colonIndex != field.length() - 1)
        fieldValue = field.substr( field.find_first_not_of(space, colonIndex + 1), \
                                                    field.find_last_not_of(space) - field.find_first_not_of(space, colonIndex + 1) + 1);

    if (uri.type == ORIGIN \
        && fieldName == "host" && fieldValue.length())
        storeHost(fieldValue, uri);

    hash.insert(make_pair<string, string>(fieldName, fieldValue));
}