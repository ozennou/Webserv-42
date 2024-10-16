/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Tokenizer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mozennou <mozennou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:57:29 by mozennou          #+#    #+#             */
/*   Updated: 2024/10/16 17:22:44 by mozennou         ###   ########.fr       */
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
    HASH,
    WHITE_SPACE
} ;

string  get_special(int special);