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