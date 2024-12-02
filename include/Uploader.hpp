/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uploader.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 17:16:06 by mlouazir          #+#    #+#             */
/*   Updated: 2024/12/02 08:36:32 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "header.hpp"

class Uploader
{
private:
    int         uploadeState;
    int         clientFd;
    size_t      maxPayloadSize;

    ofstream    ofs;

    string      buffer; // buffer
    int         state;

    size_t      totalLength;
    size_t      currentLength;
    string      fileType;
    string      boundary;
    string      closingBoundary;

    string      boundaryPart;
    int         boundaryState;

    bool        isChunked;
    bool        isMulti;
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
    void    setFileType( string type );
    void    setMaxPayloadSize( size_t payloadSize );
    void    setOfs( string uploadPath );
    void    setUploadState( int state );

    int     getUploadState( );
    void    boundaryChecks( string& payload );
    int     readHeaders( string& payload );
    void    parceHeaders( string& field );

    void    multipart( string& buffer );
    void    decodeChunked( );
    void    read( );

    void    closeUploader( );

    void    reset( );
};