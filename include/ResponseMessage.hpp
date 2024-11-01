/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMessage.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 21:10:15 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/01 20:55:36 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#include "MessageHeaders.hpp"
#include "RequestParser.hpp"

using namespace std;

class ResponseMessage
{
    int statusCode;

    string reasonPhrase;

    MessageHeaders* headers;

    // Payload

    void insertHeaders( );
public:
    ResponseMessage();

    // void generateResponse( );

    ~ResponseMessage( );
};