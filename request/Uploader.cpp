/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uploader.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/27 08:59:45 by mlouazir          #+#    #+#             */
/*   Updated: 2024/12/18 11:14:25 by mlouazir         ###   ########.fr       */
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
        this->fd = -2;
        this->state = CHUNKED_LENGTH;
        this->boundaryState = BOUNDARY;
        this->isCgi = false;
    }
    return *this;
}

Uploader::~Uploader( ) {
}

void Uploader::normalizePath( string filename ) {
    size_t j;

    // /./././././././././
    for (size_t i = 0; i < filename.length(); i++) {
        if ((filename[i] == '.' && (i > 0 && filename[i - 1] == '/')) && ((i + 1 == filename.length()) || (i + 1 < filename.length() && filename[i + 1] == '/')))
            filename.erase(i, 1);
    }
    
    // /////a/////a//////b////n
    for (size_t i = 0; i < filename.length(); i++) {
        j = i;
        for (; j < filename.length() && filename[j] == '/'; j++) {
        }
        
        if (j != i && filename[i] == '/') filename.erase(i + 1, (j - (i + 1)));
    }

    // /.. Bad Request /b1/../a1
    for (size_t i = 0; i < filename.length(); i++) {
        if ((filename[i] == '.' && (i > 0 && filename[i - 1] == '/')) \
        && (i + 1 < filename.length() && filename[i + 1] == '.') \
        && ((i + 2 == filename.length()) || (i + 2 < filename.length() && filename[i + 2] == '/'))) {
            
            if (i == 1 && filename[0] == '/') throw RequestParser::HttpRequestException("Invalid filename", 400);

            int a = i - 2;
            for (; a > 0; a--)
                if (filename[a] == '/') break;

            if (a == -1) a = 0;
            
            filename = filename.erase(a + 1, (i + 2) - a);
            i = 0;
        }
    }

    if (!filename.length()) throw RequestParser::HttpRequestException("Invalid File Name", 400);
}

void    Uploader::setBuffer( string& stringBuffer ) {
    buffer = stringBuffer;

    if (isChunked) {
        decodeChunked();

        maxPayloadSize -= buffer.length();
        if (maxPayloadSize <= 0) throw RequestParser::HttpRequestException("Max Payload Exceded", 413);

        return ;
    } else if (isMulti && !isCgi) {
        multipart(buffer);

        maxPayloadSize -= buffer.length();
        if (maxPayloadSize <= 0) throw RequestParser::HttpRequestException("Max Payload Exceded", 413);
        
        return ;
    }

    currentLength = buffer.length();
    
    if (write(fd, buffer.c_str(), buffer.length()) == -1) throw RequestParser::HttpRequestException("Can't Write To The File", 500);
    buffer.clear();
    if (currentLength >= totalLength) closeUploader();
}

void    Uploader::setTotalLength( size_t& contentLengthh ) {
    if (contentLengthh < 0) throw  RequestParser::HttpRequestException("Invalid Content-Length Value - Negative", 400);

    totalLength = contentLengthh;
}

void    Uploader::setBoundary( string boundaryValue ) {
    if (boundaryValue[0] == '"' && boundaryValue[boundaryValue.length() - 1] == '"') {
        boundaryValue.erase(0);
        boundaryValue.erase(boundaryValue.length() - 1);
    }

    if (boundaryValue.length() > 70) throw RequestParser::HttpRequestException("Boundary Too Long", 400);

    this->boundary = CRLF;
    this->boundary += "--";
    this->boundary += boundaryValue;
    this->boundary += CRLF;

    this->closingBoundary = CRLF;
    this->closingBoundary += "--";
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

void    Uploader::setIsCgi( bool value ) {
    isCgi = value;
}

void    Uploader::setFileType( string type ) {
    fileType = ".";
    fileType += type;
}

void    Uploader::setUploadPath( string uploadPathh ) {
    this->uploadPath = uploadPathh; // TODO: Give Amin Normilze Path
}

void    Uploader::setUploadState( int statee ) {
    uploadeState = statee;
}

void    Uploader::setMaxPayloadSize( size_t payloadSize ) {
    this->maxPayloadSize = payloadSize * 1000000;
}

void    Uploader::setOfs( string& filename ) {
    stringstream fullPath;

    fullPath << uploadPath;
    if (fullPath.str()[fullPath.str().length() - 1] != '/') fullPath << '/';

    fullPath << filename;
    
    normalizePath(fullPath.str());
    
    if (fd != -2) close(fd);
    
    fd = open(fullPath.str().c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);
    
    if (fd == -1) throw RequestParser::HttpRequestException("Failed To Open The Requested File-1", 500);
}

void    Uploader::setOfs( ) {
    stringstream fullPath;

    if (isCgi) {
        fullPath << "/tmp/";
    } else fullPath << uploadPath;

    if (fullPath.str()[fullPath.str().length() - 1] != '/') fullPath << '/';

    fullPath << clientFd;
    fullPath << '-';
    
    srand(time(NULL));
    int r = rand();

    fullPath << r;

    fullPath << fileType;

    if (fd != -2) close(fd);

    fd = open(fullPath.str().c_str(), O_CREAT | O_TRUNC | O_WRONLY, 0644);

    if (fd == -1) throw RequestParser::HttpRequestException("Failed To Open The Requested File-2", 500);

    if (isCgi) file = fullPath.str();
}

int    Uploader::getUploadState( void ) {
    return uploadeState;
}

bool    Uploader::getIsMulti( void ) {
    return isMulti;
}
 
void    Uploader::closeUploader( ) {
    close(fd);
    uploadeState = UPLOADED;
}

int    Uploader::parceHeaders( string& field ) {
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
        
        if (pos == string::npos) setOfs();
        else {
            string filename = fieldValue.substr(pos).substr(10); filename.erase(filename.length() - 1);

            if (filename.length()) setOfs(filename);
            else setOfs();
        }

        if (fieldValue.find(';') != string::npos) return fieldValue.substr(0, fieldValue.find(';')) == "form-data";
        else return fieldValue == "form-data";
    }
    return 0;
}

int    Uploader::readHeaders( string& payload ) {
    int flag = 0;
    while (1) {
        if (!payload.length() || payload.find(CRLF) == string::npos) return 1;

        size_t pos = payload.find(CRLF);
        string field = payload.substr(0, pos);

        pos += 2;
        
        if (!field.length()) {
            if (!flag) throw RequestParser::HttpRequestException("Invalid Content-Disposition Header", 400);

            payload = payload.substr(pos);
            return 0;
        }

        if (parceHeaders(field)) flag = 1;
        payload = payload.substr(pos);
    }
}

void    Uploader::multipart( string& payload ) {
    if (payload == CRLF && totalLength == 2) return;

    while (true) {
        if (boundaryState == BOUNDARY && !payload.rfind(closingBoundary, 0)) {
            closeUploader();
            break ;
        }
        if (boundaryState == BOUNDARY && (!payload.rfind(boundary, 0) || !payload.rfind(boundary.substr(2), 0))) {
            if (!payload.find(CRLF)) payload = payload.substr(boundary.length());
            else payload = payload.substr(boundary.length() - 2);
            
            boundaryState = HEADERS;
        } else if (boundaryState == HEADERS) {
            if (readHeaders(payload)) break ;

            boundaryState = BOUNDARY;
        }
        else {
            if (!payload.length()) break ;
            
            if (payload.find(closingBoundary) != string::npos || payload.find(boundary) != string::npos) {
                size_t pos;

                if (payload.find(closingBoundary) != string::npos) pos = payload.find(closingBoundary);
                else pos = payload.find(boundary);
                
                string tmp = payload.substr(0, pos);
                
                if (write(fd, tmp.c_str(), tmp.length()) == -1) throw RequestParser::HttpRequestException("Can't Write To The File", 500);
                
                payload = payload.substr(pos);
            } else {
                if (write(fd, payload.c_str(), payload.length()) == -1) throw RequestParser::HttpRequestException("Can't Write To The File", 500);
                break ;
            }
        }
    }
    payload.clear();
}

void    Uploader::decodeChunked( ) {
    string payload = "";
    size_t payloadLength = 0;

    while (true) {
        if (state == CHUNKED_LENGTH && buffer.find(CRLF) != string::npos) {
            totalLength = 0; currentLength = 0;
            stringstream ss;
            
            ss << hex << buffer.substr(0, buffer.find(CRLF));
            ss >> totalLength;

            if (ss.fail() || totalLength < 0) throw RequestParser::HttpRequestException("Bad Hex Digit", 400);

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
            
            if (isMulti && !isCgi) multipart(payload);
            else if (write(fd, payload.c_str(), payload.length()) == -1) throw RequestParser::HttpRequestException("Can't Write To The File", 500);

            if ((currentLength + buffer.length() > totalLength) && ((totalLength - currentLength) + 2 < buffer.length())) buffer = buffer.substr((totalLength - currentLength) + 2);
            else buffer.clear();

            maxPayloadSize -= payloadLength;

            if (maxPayloadSize <= 0) throw RequestParser::HttpRequestException("Max Payload Exceded", 413);

            state = CHUNKED_LENGTH;
        }
        else if (state == CHUNKED_DATA) {
            payload = buffer;
            
            if (isMulti && !isCgi) multipart(payload);
            else if (write(fd, payload.c_str(), payload.length()) == -1) throw RequestParser::HttpRequestException("Can't Write To The File", 500);

            currentLength += buffer.length();
            maxPayloadSize -= buffer.length();

            if (maxPayloadSize <= 0) throw RequestParser::HttpRequestException("Max Payload Exceded", 413);

            buffer.clear(); payload.clear();
            break;
        }
        else break;
    }
}

void    Uploader::read( ) {
    int i;
    char buf[200000];

    i = recv(clientFd, buf, 200000, 0);
    
    if (!i) throw RequestParser::HttpRequestException("Connection Ended", 0);
    if (i == -1) throw RequestParser::HttpRequestException("Nothing Yet", -1);
    
    buffer.append(buf, i);

    if (isChunked) {
        decodeChunked();
        buffer.clear();
        return ;
    }
    
    currentLength += i;
    maxPayloadSize -= i;

    if (maxPayloadSize <= 0) {cout << "Max Exceeded" << endl; throw RequestParser::HttpRequestException("Max Payload Exceded", 413);}
    
    if (isMulti && !isCgi) multipart(buffer);
    else {
        if (write(fd, buffer.c_str(), buffer.length()) == -1) throw RequestParser::HttpRequestException("Can't Write To The File", 500);
        if (currentLength >= totalLength) closeUploader();
    }

    buffer.clear();
}

void    Uploader::reset( ) {
    uploadeState = UPLOADED;
    buffer.clear();
    writingBuffer.clear();
    state = CHUNKED_LENGTH;
    boundaryState = BOUNDARY;
    totalLength = 0;
    currentLength = 0;
    fileType.clear();
    boundary.clear();
    closingBoundary.clear();
    boundaryPart.clear();
    uploadPath.clear();
    isChunked = false;
    isMulti = false;
    fd = -2;
    maxPayloadSize = 0;
}