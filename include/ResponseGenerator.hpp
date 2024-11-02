/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 18:44:59 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 14:50:21 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ResponseMessage.hpp"

#include <map>

class ResponseGenerator
{
    int requestState;

    int statusCode;

    ResponseMessage* responseMessage;

    map<int, string> errorHashMap;
public:
    ResponseGenerator( ResponseMessage* responseMessage );

    void setRequestState( int requestState, int statusCode );

    void generateResponse();

    ~ResponseGenerator();
};