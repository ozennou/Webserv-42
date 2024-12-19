#include "header.hpp"

int is_special_token(char a)
{
    if (isspace(a))
        return WHITE_SPACE;
    if (a == '{')
        return OPEN_BRACKET;
    if (a == '}')
        return CLOSE_BRACKET;
    if (a == ';')
        return SEMICOLON;
    if (a == ':')
        return COLON;
    if (a == '\'')
        return SINGLE_QUOTE;
    if (a == '"')
        return DOUBLE_QUOTE;
    if (a == '=')
        return EQUAL;
    if (a == ',')
        return COMMA;
    if (a == '#')
        return HASH;
    return TOKEN;
}

string  get_special(int special)
{
    if (special == OPEN_BRACKET)
        return "{";
    if (special == CLOSE_BRACKET)
        return "}";
    if (special == SEMICOLON)
        return ";";
    if (special == COLON)
        return ":";
    if (special == EQUAL)
        return "=";
    if (special == COMMA)
        return ",";
    if (special == SINGLE_QUOTE)
        return "'";
    if (special == DOUBLE_QUOTE)
        return "\"";
    return "TOKEN";
}

string  get_token(string &line, size_t& index, int sp = -1)
{
    size_t  old_index = index;
    while (line[index])
    {
        if (sp == SINGLE_QUOTE && line[index] == '\'')
            break ;
        if (sp == DOUBLE_QUOTE && line[index] == '"')
            break ;
        if (sp == -1 && (isspace(line[index]) || is_special_token(line[index])))
            break ;
        index++;
    }
    if (!line[index] && sp != -1)
        throw logic_error("Error: quote not found :" + line);
    return line.substr(old_index, index - old_index);
}

vector<pair<int, string> >  tokenizer(ifstream& conf_file)
{
    string                      line;
    size_t                      line_size;
    vector<pair<int, string> >   res;
    int                         special, brackets = 0;

    while (getline(conf_file, line))
    {
        line_size = line.size();
        for (size_t i = 0; i < line_size; i++)
        {
            special = is_special_token(line[i]);
            if (special == WHITE_SPACE)
                continue;
            else if (special == HASH)
                break ;
            else if (special == DOUBLE_QUOTE || special == SINGLE_QUOTE)
                res.push_back(pair<int, string>(TOKEN_IN_QUOTES, get_token(line, ++i, special)));
            else if (special)
                res.push_back(pair<int, string>(special, get_special(special)));
            else
            {
                res.push_back(pair<int, string>(TOKEN, get_token(line, i)));
                special = is_special_token(line[i]);
                if (special && special != WHITE_SPACE)
                    res.push_back(pair<int, string>(special, get_special(special)));
            }
        }
    }
    for (vector<pair<int, string> >::iterator i = res.begin(); i != res.end(); i++)
    {
        if ((*i).first == OPEN_BRACKET)
            brackets++;
        else if ((*i).first == CLOSE_BRACKET)
            brackets--;
    }
    if (brackets)
        throw logic_error("Error: Bracket not found");
    return res;
}