/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uploader.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 08:59:45 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/30 11:34:24 by mlouazir         ###   ########.fr       */
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

void    Uploader::setOfs( string uploadPath ) {
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
 
void    Uploader::closeUploader( ) {
    ofs.close(); ofs.clear();
    uploadeState = UPLOADED;
}

void    Uploader::decodeChunked( ) {
    
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
            
            string payload = buffer.substr(0, (totalLength - currentLength));
            
            ofs.write(payload.c_str(), payload.length());

            if ((currentLength + buffer.length() > totalLength) && ((totalLength - currentLength) + 2 < buffer.length()))
                buffer = buffer.substr((totalLength - currentLength) + 2);
            else buffer.clear();

            currentLength += payload.length();

            state = CHUNKED_LENGTH;
        }
        else if (state == CHUNKED_DATA) {
            ofs.write(buffer.c_str(), buffer.length());
            currentLength += buffer.length();
            buffer.clear();
            break;
        }
        else break;
    }
}

void    Uploader::read( ) {
    int i;
    char buf[10001];

    i = recv(clientFd, buf, 10000, 0);
    
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