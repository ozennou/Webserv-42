/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 21:10:15 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/30 21:37:18 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

#include "Headers.hpp"
#include "RequestParser.hpp"

using namespace std;

class Response
{
    string responseMessage;

    int statusCode;

    string reasonPhrase;

    Headers* headers;


    void insertHeaders( );
public:
    Response();

    void generateResponse( );

    ~Response( );
};