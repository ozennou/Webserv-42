/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uploader.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 08:59:45 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/27 19:35:07 by mlouazir         ###   ########.fr       */
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
    }
    return *this;
}

Uploader::~Uploader( ) {
}

void    Uploader::setBuffer( string& stringBuffer ) {
    buffer = stringBuffer;
}

void    Uploader::setTotalLength( size_t& contentLengthh ) {
    totalLength = contentLengthh;
    currentLength = buffer.length();
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

    // cout << fullPath.str() << endl;
    if (!ofs.is_open()) throw RequestParser::HttpRequestException("Failed To Open The Requested File", 500);
}

void    Uploader::setUploadState( int state ) {
    uploadeState = state;
}

int    Uploader::getUploadState( void ) {
    return uploadeState;
}

void    Uploader::closeUploader( ) {
    ofs.close(); ofs.clear();
    uploadeState = UPLOADED;
}

void    Uploader::read( ) {
    if (buffer.length()) {
        ofs.write(buffer.c_str(), buffer.length()); buffer.clear();
        if (currentLength >= totalLength) closeUploader();
    }

    int i;
    char buf[5621];

    i = recv(clientFd, buf, 5620, 0);

    if (!i) throw RequestParser::HttpRequestException("Connection Ended", 0);
    if (i == -1) throw RequestParser::HttpRequestException("Nothing Yet", -1);

    buffer.append(buf, i);

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