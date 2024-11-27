/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uploader.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 17:16:06 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/27 16:51:37 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "header.hpp"

class Uploader
{
private:
    int         uploadeState;
    int         clientFd;

    ofstream    ofs;

    string      buffer; // buffer

    size_t      totalLength;
    size_t      currentLength;
    string      fileType;

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

    void    setIsChunked( bool value );
    void    setIsMulti( bool value );
    void    setFileType( string type );
    void    setOfs( string uploadPath );
    void    setUploadState( int state );
    int     getUploadState( );

    void    read( );

    void    closeUploader( );

    void    reset( );
};