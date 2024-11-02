/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMachine.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 16:13:45 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 15:24:42 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ResponseGenerator.hpp"

class ResponseMachine
{
    ResponseGenerator   responseGenerator;
public:
    ResponseMachine( int clientFd );

    void    setRequestState( int requestState, int statusCode );

    void    generateResponse();
    ~ResponseMachine( );
};