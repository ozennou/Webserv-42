/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uploader.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 17:16:06 by mlouazir          #+#    #+#             */
/*   Updated: 2024/12/21 11:47:46 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "header.hpp"

class Uploader
{
private:
    int         uploadeState;
    int         clientFd;
    long long      maxPayloadSize;

    string      buffer; // buffer
    string      writingBuffer;
    int         state;

    size_t      totalLength;
    size_t      currentLength;
    string      fileType;
    string      boundary;
    string      closingBoundary;

    string      boundaryPart;
    int         boundaryState;

    string      uploadPath;

    bool        isChunked;
    bool        isMulti;
    bool        isCgi;

    vector<string> filenames;
public:
    Uploader( );
    Uploader( int& clientFd );
    Uploader( const Uploader& obj );
    Uploader& operator=( const Uploader& obj );

    ~Uploader();

    void    setBuffer( string& stringBuffer );
    void    setTotalLength( size_t& totalLengthh );
    void    setBoundary( string boundaryValue );
    void    setIsChunked( bool value );
    void    setIsMulti( bool value );
    void    setIsCgi( bool value );
    void    setFileType( string type );
    void    setMaxPayloadSize( size_t payloadSize );
    void    setOfs( );
    void    setOfs( string& filename );
    void    setUploadPath( string uploadPathh );
    void    setUploadState( int state );

    int     getUploadState( );
    bool    getIsMulti( );
    int     readHeaders( string& payload );
    int     parceHeaders( string& field );
    void    normalizePath( string path );

    void    multipart( string& buffer );
    void    decodeChunked( );
    void    read( );

    void    closeUploader( );

    void    reset( );

    int         fd;
    string      file;
};