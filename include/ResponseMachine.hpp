/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseMachine.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/01 16:13:45 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/02 12:27:06 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ResponseMessage.hpp"

#include "ResponseGenerator.hpp"

class ResponseMachine
{
    ResponseMessage* responseMessage;

    ResponseGenerator responseGenerator;
public:
    ResponseMachine();
    ~ResponseMachine();
};