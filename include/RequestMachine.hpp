/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMachine.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:19:07 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/01 21:39:31 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQ_MACHI
#define REQ_MACHI
// #pragma once

#include "RequestParser.hpp"

#include "RequestMessage.hpp"

enum {
    BAD,
    GOOD,
    PROCESSING,
    FINISEHD
};

class RequestMachine
{
private:

    RequestMessage *requestMessage;

    RequestParser requestParser;
public:
    RequestMachine( int clientFd );

    void initFromLastPoint( void );

    ~RequestMachine( );
};

#endif
