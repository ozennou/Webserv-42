/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uploader.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 08:59:45 by mlouazir          #+#    #+#             */
/*   Updated: 2024/12/03 07:51:32 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Uploader.hpp"
#include "../include/RequestParser.hpp"

Uploader::Uploader( ) {
    uploadeState = UPLOADED;
}

Uploader::Uploader( int& clientFd ) : uploadeState(UPLOADED), clientFd(clientFd) {

}

Uploader::Uploader( const Uploader& obj ) {
    *this = obj;
}

Uploader& Uploader::operator=( const Uploader& obj ) {
    if (this != &obj) {
        this->clientFd = obj.clientFd;
        this->totalLength = 0;
        this->currentLength = 0;
        this->state = CHUNKED_LENGTH;
        this->boundaryState = BOUNDARY;
    }
    return *this;
}

Uploader::~Uploader( ) {
}

void    Uploader::setBuffer( string& stringBuffer ) {
    buffer = stringBuffer;

    if (isChunked) {
        decodeChunked();
        return ;
    }

    currentLength = buffer.length();
    
    ofs.write(buffer.c_str(), buffer.length()); buffer.clear();
    if (currentLength >= totalLength) closeUploader();
}

void    Uploader::setTotalLength( size_t& contentLengthh ) {
    totalLength = contentLengthh;
}

void    Uploader::setBoundary( string boundaryValue ) {
    if (boundaryValue.length() > 70) throw RequestParser::HttpRequestException("Boundary Too Long", 400);

    this->boundary = "--";
    this->boundary += boundaryValue;
    this->boundary += CRLF;

    this->closingBoundary = CRLF;
    this->closingBoundary = "--";
    this->closingBoundary += boundaryValue;
    this->closingBoundary += "--";
    this->closingBoundary += CRLF;
}

void    Uploader::setIsChunked( bool value ) {
    isChunked = value;
}

void    Uploader::setIsMulti( bool value ) {
    isMulti = value;
}

void    Uploader::setFileType( string type ) {
    fileType = ".";
    fileType += type;
}

void    Uploader::setUploadPath( string uploadPathh ) {
    this->uploadPath = uploadPathh;
}

void    Uploader::setOfs( string& filename ) {
    // Basic filename checks here
    stringstream fullPath;

    fullPath << uploadPath;
    if (fullPath.str()[fullPath.str().length() - 1] != '/') fullPath << '/';

    fullPath << filename;
    
    ofs.open(fullPath.str());

    if (!ofs.is_open()) throw RequestParser::HttpRequestException("Failed To Open The Requested File", 500);
}

void    Uploader::setOfs( ) {
    stringstream fullPath;

    fullPath << uploadPath;
    if (fullPath.str()[fullPath.str().length() - 1] != '/') fullPath << '/';

    fullPath << clientFd;
    fullPath << '-';
    
    srand(time(NULL));
    int r = rand();

    fullPath << r;

    fullPath << fileType;

    ofs.open(fullPath.str());

    if (!ofs.is_open()) throw RequestParser::HttpRequestException("Failed To Open The Requested File", 500);
}

void    Uploader::setUploadState( int statee ) {
    uploadeState = statee;
}

void    Uploader::setMaxPayloadSize( size_t payloadSize ) {
    this->maxPayloadSize = payloadSize * 1000000;
}

int    Uploader::getUploadState( void ) {
    return uploadeState;
}

bool    Uploader::getIsMulti( void ) {
    return isMulti;
}
 
void    Uploader::closeUploader( ) {
    ofs.close(); ofs.clear();
    uploadeState = UPLOADED;
}

void    Uploader::parceHeaders( string& field ) {
    size_t colonIndex = field.find(':');
    string delimiters = DELI;
    string space = " \t";

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

    if (colonIndex != field.length() - 1) {
        size_t k = colonIndex + 1;
        for (; k < field.length(); k++) {
            if (!isspace(field[k])) break;
        }
        if (k != field.length()) {
            fieldValue = field.substr( field.find_first_not_of(space, colonIndex + 1), \
                                                        field.find_last_not_of(space) - field.find_first_not_of(space, colonIndex + 1) + 1); stolower(fieldValue);
        }
    }

    if (fieldName == "content-disposition") {
        size_t pos = fieldValue.find("filename=");

        if (pos == string::npos) return ;

        string filename = fieldValue.substr(pos).substr(10); filename.erase(filename.length() - 1);

        ofs.close(); ofs.clear();
        setOfs(filename);
    }
}

int    Uploader::readHeaders( string& payload ) {

    while (1) {
        if (!payload.length() || payload.find(CRLF) == string::npos) return 1;

        size_t pos = payload.find(CRLF);
        string field = payload.substr(0, pos);

        pos += 2;
        
        if (!field.length()) {
            payload = payload.substr(pos);
            return 0;
        }

        parceHeaders(field);
        payload = payload.substr(pos);
    }
}

void    Uploader::boundaryChecks( string& payload ) {
    if (boundary.length() >= payload.length()) return ;
    
    string  lastBufferPart = payload.substr(payload.length() - boundary.length() + 1 /* This 1 is neccesarry */);
    size_t i = 0;
    
    // This Part Is For The Boundary
    for (; i < lastBufferPart.length(); i++) {
        string  tmp = lastBufferPart.substr(i);

        if (!boundary.rfind(tmp, 0)) {
            boundaryPart = tmp;
            payload = payload.substr(0, payload.length() - boundary.length() + 1 + i);
            break;
        }
    }

    // This Part Is For The ClosingBoundary
    if (!boundaryPart.length()) {
        if (closingBoundary.length() >= payload.length()) return ;
        
        i = 0;
        lastBufferPart = payload.substr(payload.length() - closingBoundary.length() + 1 /* This 1 is neccesarry */);
        for (; i < lastBufferPart.length(); i++) {
            string  tmp = lastBufferPart.substr(i);

            if (!closingBoundary.rfind(tmp, 0)) {
                boundaryPart = tmp;
                payload = payload.substr(0, payload.length() - closingBoundary.length() + 1 + i);
                break;
            }
        }
    }
}

void    Uploader::multipart( string& payload ) {
    if (boundaryPart.length()) {
        payload.insert(0, boundaryPart);
        boundaryPart.clear();
    }

    boundaryChecks(payload);

    while (true) {
        if (boundaryState == BOUNDARY && !payload.rfind(closingBoundary, 0)) {
            closeUploader();
            return ;
        }

        if (boundaryState == BOUNDARY && !payload.rfind(boundary, 0)) {
            payload = payload.substr(boundary.length());
            
            boundaryState = HEADERS;
        } else if (boundaryState == HEADERS) {
            if (readHeaders(payload)) return ;
            
            boundaryState = BOUNDARY;
        }
        else if (boundaryState == BOUNDARY) {
            if (!payload.length()) return ;

            if (payload.find(closingBoundary) != string::npos || payload.find(boundary) != string::npos) {
                size_t pos;

                if (payload.find(closingBoundary) != string::npos) pos = payload.find(closingBoundary);
                else pos = payload.find(boundary);
                
                string tmp = payload.substr(0, pos);
                
                if (tmp!= CRLF)ofs.write(tmp.c_str(), tmp.length());

                payload = payload.substr(pos);
            } else {
                if (payload != CRLF) ofs.write(payload.c_str(), payload.length());
                return ;
            }
        }
    }
}

void    Uploader::decodeChunked( ) {
    string payload = buffer;
    size_t payloadLength = 0;

    while (true) {
        if (buffer.find(CRLF) != string::npos && state == CHUNKED_LENGTH) {
            stringstream ss;
            
            if (!buffer.find(CRLF)) buffer = buffer.substr(2);

            if (!buffer.length()) break;
            
            ss << hex << buffer.substr(0, buffer.find(CRLF));
            ss >> totalLength; currentLength = 0;

            if (ss.fail()) throw RequestParser::HttpRequestException("Bad Hex Digit", 400);
            
            if (!totalLength) {
                closeUploader();
                break;
            }

            buffer = buffer.substr(buffer.find(CRLF) + 2);

            state = CHUNKED_DATA;
        }
        else if (state == CHUNKED_DATA && (currentLength + buffer.length() >= totalLength)) {

            payload = buffer.substr(0, (totalLength - currentLength));
            payloadLength = payload.length();

            if (isMulti) multipart(payload);
            else ofs.write(payload.c_str(), payload.length());

            if ((currentLength + buffer.length() > totalLength) && ((totalLength - currentLength) + 2 < buffer.length()))
                buffer = buffer.substr((totalLength - currentLength) + 2);
            else buffer.clear();

            currentLength += payloadLength;

            state = CHUNKED_LENGTH;
        }
        else if (state == CHUNKED_DATA) {
            payload = buffer;
            
            if (isMulti) multipart(payload);
            else ofs.write(payload.c_str(), payload.length());

            currentLength += buffer.length();
            buffer.clear();
            break;
        }
        else break;
    }
}

void    Uploader::read( ) {
    int i;
    char buf[16001];

    i = recv(clientFd, buf, 16000, 0);
    
    if (!i) throw RequestParser::HttpRequestException("Connection Ended", 0);
    if (i == -1) throw RequestParser::HttpRequestException("Nothing Yet", -1);

    buffer.append(buf, i);

    if (isChunked) {
        decodeChunked();
        return ;
    }
    
    currentLength += i;

    ofs.write(buffer.c_str(), buffer.length()); buffer.clear();
    if (currentLength >= totalLength) closeUploader();
}

void    Uploader::reset( ) {
    uploadeState = UPLOADED;
    ofs.close(); ofs.clear();
    buffer.clear();
    totalLength = 0;
    currentLength = 0;
    fileType.clear();
    isChunked = false;
    isMulti = false;
}