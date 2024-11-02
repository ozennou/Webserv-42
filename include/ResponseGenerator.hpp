/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseGenerator.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 18:44:59 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 12:26:19 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ResponseMessage.hpp"

class ResponseGenerator
{
    int requestState;

    int statusCode;

    ResponseMessage* responseMessage;
public:
    ResponseGenerator( int requestState, int statusCode, ResponseMessage* responseMessage );

    ~ResponseGenerator();
};