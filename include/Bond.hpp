/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 14:47:50 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 15:26:41 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "header.hpp"

#include <iostream>

#include "RequestMachine.hpp"

#include "ResponseMachine.hpp"

class Bond
{
private:

    int requestState;

    int responseState;

    int clientFd;
    
    RequestMachine requestMachine;

    ResponseMachine responseMachine;
public:
    Bond( int clientFd );

    int getClientFd( void );

    int getRequestState( void );

    int getResponseState( void );

    void initParcer( void );

    void initBuilder( void );

    // void reset( );

    ~Bond( );
};
