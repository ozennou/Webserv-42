/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestMessage.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:39:37 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/29 14:45:29 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <map>

#include "Uri.hpp"
#include "MessageHeaders.hpp"

// class Uri;

enum {
    GET,
    POST,
    DELETE
};

class RequestMessage
{
    // -> RequestMessage line
    
public:
    int         methode;
    Uri*        uri;
    MessageHeaders*    headers;
    //

    // -> header section
    std::map<std::string, std::string> headerFields;
    //

    RequestMessage();
    ~RequestMessage();
};