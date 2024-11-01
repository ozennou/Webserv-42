/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMachine.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:19:07 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/01 15:41:18 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "RequestParser.hpp"

#include "RequestMessage.hpp"

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