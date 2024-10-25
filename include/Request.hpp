/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:39:37 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/24 14:57:08 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <map>

enum {
    GET,
    POST,
    DELETE
};

class Request
{
    // -> request line
    
public:
    int         methode;
    std::string targetUrl;
    std::string httpVersion;
    //

    // -> header section
    std::map<std::string, std::string> headerFields;
    //

    Request();
    ~Request();
};