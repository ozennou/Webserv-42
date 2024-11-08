/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 14:47:50 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/04 20:41:51 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "header.hpp"

#include <iostream>

#include "RequestParser.hpp"

#include "ResponseGenerator.hpp"

class Bond
{
private:
    int clientFd;

    RequestParser requestParser;

    ResponseGenerator responseGenerator;
public:
    Bond( int clientFd, int socketFd, Socket_map& socket_map );

    int getClientFd( void );

    void initParcer( void );

    void initBuilder( void );

    // void getPayLoadFromParser( void );

    ~Bond( );
};
