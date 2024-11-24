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

MessageHeaders::MessageHeaders( ) : rangeType(NO_RANGE), delimiters(DELI), space(" \t") {
}

MessageHeaders::MessageHeaders( const MessageHeaders& obj ) {
    *this = obj;
}

MessageHeaders& MessageHeaders::operator=( const MessageHeaders& obj ) {
    if (this != &obj) {
        this->rangeType = obj.rangeType;
        this->first = obj.first;
        this->last = obj.last;
        this->delimiters = obj.delimiters;
        this->space = obj.space;
        this->headers = obj.headers;
    }
    return *this;
}

MessageHeaders::~MessageHeaders( ) {
}


map<string, string>::iterator MessageHeaders::findContentHeaders( ) {
    map<string, string>::iterator it = headers.find("Transfer-Encoding");
    if (it == headers.end()) it = headers.find("Content-Length");
    if (it == headers.end()) throw RequestParser::HttpRequestException("No Content Related Headers Found", 400);
    return it;
}

void MessageHeaders::parceFieldValue( ) {
    for (multimap<string, string>::iterator it = headers.begin(); it != headers.end(); it++) {
        for (size_t i = 0; i < it->second.length(); i++) {
            int character = it->second[i];
            if ((character < 0 || character > 255) \
            || (!isprint(character) && space.find(character) == string::npos))
                throw RequestParser::HttpRequestException("Invalid Character Found in the field-value", 400);
        }
    }
}

int MessageHeaders::getRangeType( ) {
    return rangeType;
}

string MessageHeaders::getRangeFirst( ) {
    return first;
}

string MessageHeaders::getRangeLast( ) {
    return last;
}

bool MessageHeaders::findField( string fieldName ) {
    map<string, string>::iterator it = headers.find(fieldName);
    if (it == headers.end()) return false;
    return true;
}

bool MessageHeaders::connectionState( ) {
    map<string, string>::iterator it = headers.find("connection");
    if (it == headers.end()) return true;
    stolower(it->second);
    if (it->second == "close") return false;
    return true;
}

size_t  getRangeValue( string rangeString ) {
    if (!rangeString.length()) return 0;

    stringstream ss;
    ss << rangeString;

    size_t rangeValue;
    ss >> rangeValue;

    return rangeValue;
}

bool MessageHeaders::isValidRange( Uri& uri ) {

    size_t resourceSize = uri.getResourceSize();

    if (rangeType == INT_RANGE) {
        size_t rangeFirst = getRangeValue(first);
        size_t rangeLast;

        if (last.length()) rangeLast = getRangeValue(last);
        else rangeLast = resourceSize;
        if (rangeLast > resourceSize) rangeLast = resourceSize;

        if (rangeFirst >= resourceSize) return false;
        if (rangeFirst >= rangeLast) return false;
    } else {
        size_t rangeLast = getRangeValue(last);

        if (resourceSize <= rangeLast) return false;
        if (!rangeLast) return false;
    }
    return true;
}

void MessageHeaders::storeRange( string& fieldValue ) {
    if (!fieldValue.length()) return ;

    if (fieldValue.compare(0, 6, "bytes=")) return ;

    string rangeSet = fieldValue.substr(6);
    if (rangeSet[0] == '-') {
        for (size_t i = 1; i < rangeSet.length(); i++) {
            if (!isdigit(rangeSet[i])) return ;
        }
        last = rangeSet.substr(1);
        rangeType = SUFFIX_RANGE;
    } else if (isdigit(rangeSet[0])) {
        size_t i = 0;
        for (; i < rangeSet.length(); i++) {
            if (!isdigit(rangeSet[i]) && rangeSet[i] != '-') return ;
            if (rangeSet[i] == '-') break;
        }
        
        first = rangeSet.substr(0, i);
        last = "";
        if (i != rangeSet.length()) {
            size_t a = i + 1;
            for (; a < rangeSet.length(); a++) {
                if (!isdigit(rangeSet[a])) return ;
            }
            last = rangeSet.substr(i + 1, (rangeSet.length() - (i + 1)));
        }
        rangeType = INT_RANGE;
    } else
        return ;
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

        ss << fieldValue.substr(fieldValue.find(':') + 1, fieldValue.length() - (fieldValue.find(':') + 1)); ss >> uri.port;
        
        if (ss.fail() || uri.port < 0 || uri.port > 65535) throw RequestParser::HttpRequestException("Invalid Port Number", 400);
    }
}

void MessageHeaders::storeField( string& field, Uri& uri, int method ) {
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

    if (method == GET && fieldName == "range") storeRange(fieldValue);

    headers.insert(make_pair<string, string>(fieldName, fieldValue));
}