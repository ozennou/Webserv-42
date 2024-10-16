/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mozennou <mozennou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:57:29 by mozennou          #+#    #+#             */
/*   Updated: 2024/10/14 20:55:02 by mozennou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

enum {
    TOKEN,
    TOKEN_IN_QUOTES,
    OPEN_BRACKET,
    CLOSE_BRACKET,
    SEMICOLON,
    COLON,
    SINGLE_QUOTE,
    DOUBLE_QUOTE,
    COMMA,
    EQUAL,
    WHITE_SPACE
} ;

string  get_special(int special);