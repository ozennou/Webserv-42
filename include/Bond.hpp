/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bond.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 14:47:50 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/01 21:01:01 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#include "RequestMachine.hpp"

#include "ResponseMachine.hpp"

class Bond
{
private:

    int state;

    int clientFd;
    
    RequestMachine requestMachine;

    // ResponseMachine
public:
    Bond( int clientFd );

    int getClientFd( void );

    void initParcing( void );

    ~Bond( );
};
