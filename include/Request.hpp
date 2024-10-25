/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/21 14:39:37 by mlouazir          #+#    #+#             */
/*   Updated: 2024/10/25 21:39:11 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <map>

// #include "Uri.hpp"

// class Uri;

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
    // Uri         uri;
    std::string httpVersion;
    //

    // -> header section
    std::map<std::string, std::string> headerFields;
    //

    Request();
    ~Request();
};