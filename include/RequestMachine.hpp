/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMachine.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/31 15:19:07 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 15:12:28 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

class RequestParser;

#include "RequestParser.hpp"

class RequestMachine
{
private:

    RequestParser requestParser;
public:
    RequestMachine( int clientFd );

    void initFromLastPoint( void );

    ~RequestMachine( );
};
