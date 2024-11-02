/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 18:44:59 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 15:25:20 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <map>
#include <header.hpp>

#include "MessageHeaders.hpp"

class ResponseGenerator
{
    // Response Component
    int statusCode;

    string reasonPhrase;

    MessageHeaders* headers;
    //

    // Buffer
    string  responseBuffer;

    // 
    int requestState;

    map<int, string> errorHashMap;

    //
    int clientFd;
public:
    ResponseGenerator( int clientFd );

    void setRequestState( int requestState, int statusCode );

    void generateResponse();

    ~ResponseGenerator();
};