/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_location.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mozennou <mozennou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 22:19:32 by mozennou          #+#    #+#             */
/*   Updated: 2024/10/15 11:24:42 by mozennou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <header.hpp>

void    check_equal(vector<pair<int, string> >::iterator &i)
{
    if ((++i)->first != EQUAL)
        throw logic_error("Error: wrong format (location: DIRECTIVE=VALUE)");
    if ((++i)->first != TOKEN && i->first != TOKEN_IN_QUOTES)
        throw logic_error("Error: wrong format (location: DIRECTIVE=VALUE)");
}

void    parse_directive(vector<pair<int, string> >::iterator &i, vector<pair<int, string> >::iterator end, Location& res)
{
    if (i->second == "ROUTE")
    {
        check_equal(i);
        res.setRoute((i++)->second);
    }
    else if (i->second == "METHODS")
    {
        check_equal(i);
        while (i != end && (i->first == TOKEN || i->first == TOKEN_IN_QUOTES))
            res.addMethod((i++)->second);
    }
    else if (i->second == "INDEX")
    {
        check_equal(i);
        while (i != end && (i->first == TOKEN || i->first == TOKEN_IN_QUOTES))
            res.addDefaultPage((i++)->second);
    }
    else if (i->second == "ROOT")
    {
        check_equal(i);
        res.setRoot((i++)->second);
    }
    else if (i->second == "DIRLST")
    {
        check_equal(i);
        if (i->second == "on" || i->second == "yes" || i->second == "y")
            res.setDirListings(true);
        else if (i->second == "off" || i->second == "no" || i->second == "n")
            res.setDirListings(false);
        else
            throw logic_error("Error: invalid DIRLST option :" + i->second);
        i++;
    }
    else if (i->second == "UPLOAD")
    {
        check_equal(i);
        res.setUploadPath((i++)->second);
    }
    else if (i->second == "CGI_EXT")
    {
        check_equal(i);
        while (i != end && (i->first == TOKEN || i->first == TOKEN_IN_QUOTES))
            res.addCgiExt((i++)->second);
    }
    else if (i->second == "CGI_TIMEOUT")
    {
        check_equal(i);
        unsigned int    timeout;
        stringstream    ss(i->second);
        ss >> timeout;
        if (ss.fail() || !ss.eof() || i->first == TOKEN_IN_QUOTES)
            throw logic_error("Error: invalid CGI timeout :" + i->second);
        res.setCgiTimeout(timeout);
        i++;
    }
    else
        throw logic_error("Error: invalid directive :" + i->second);
}

Location    parse_location(vector<pair<int, string> >::iterator &i, vector<pair<int, string> >::iterator end)
{
    Location    res;
    i = i + 2;
    while (i != end && i->first != CLOSE_BRACKET)
    {
        parse_directive(i, end, res);
        if (i->first == SEMICOLON)
            break ;
        if (i->first != COMMA)
            throw logic_error("Error: location directives separated bu ','");
        i++;
    }
    return res;
}