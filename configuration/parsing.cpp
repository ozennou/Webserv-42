/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mozennou <mozennou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 17:31:41 by mozennou          #+#    #+#             */
/*   Updated: 2024/10/15 14:23:02 by mozennou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <header.hpp>

void    check_is_root(void)
{
    uid_t   uid = getuid();
    struct passwd *pw = getpwuid(uid);
    string      res(pw->pw_name);
    if (res != "root")
        throw logic_error("Error: need root privileges for port range from 1 to 1024");
}

void    parse_ports(vector<pair<int, string> >::iterator &i, vector<pair<int, string> >::iterator end, Server &srv)
{
    i = i + 2;
    while (i != end && i->first != CLOSE_BRACKET)
    {
        int             port;
        stringstream    ss(i->second);
        ss >> port;
        if (ss.fail() || !ss.eof() || port <= 0 || port > 65535 || i->first != TOKEN)
            throw logic_error("Error: not valid port :" + i->second);
        if (port > 0 && port < 1024)
            check_is_root();
        srv.addPort(port);
        if ((++i) != end && i->first == SEMICOLON)
            break ;
        if (i == end || i->first != COMMA)
            throw logic_error("Error: ',' between ports");
        i++;
    }
}

void    parse_server_names(vector<pair<int, string> >::iterator &i, vector<pair<int, string> >::iterator end, Server &srv)
{
    i = i + 2;
    while (i != end && i->first != CLOSE_BRACKET)
    {
        srv.addServerName(i->second);
        if ((++i) != end && i->first == SEMICOLON)
            break ;
        if (i == end || i->first != COMMA)
            throw logic_error("Error: ',' between server names");
        i++;
    }
}

void    parse_error_pages(vector<pair<int, string> >::iterator &i, vector<pair<int, string> >::iterator end, Server &srv)
{
    int error_code;
    i = i + 2;

    while (i != end && i->first != CLOSE_BRACKET)
    {
        stringstream ss(i->second);
        ss >> error_code;
        if (ss.fail() || !ss.eof() || error_code < 300 || error_code > 599)
            throw logic_error("Error: error code number (must be between 300 and 599) :" + i->second);
        if (++i == end || i->first != EQUAL)
            throw logic_error("Error: wrong formay (error_code=error_page) :");
        if (++i == end || (i->first != TOKEN && i->first != TOKEN_IN_QUOTES))
            throw logic_error("Error: wrong formay (error_code=error_page) :");
        srv.setErrorPage(error_code, i->second);
        if (++i != end && i->first == SEMICOLON)
            break ;
        i++;
    }
}

void    parse_body_size(vector<pair<int, string> >::iterator &i, Server &srv)
{
    size_t  body_size;
    i = i + 2;
    stringstream    ss(i->second);
    ss >> body_size;
    
    if (ss.fail() || !ss.eof() || i->first != TOKEN)
        throw logic_error("Error: body size error :" + i->second);
    srv.setBodySize(body_size);
    i++;
}

int    parse_directive(vector<pair<int, string> >::iterator &i, vector<pair<int, string> >::iterator end, Server &srv)
{
    if (i->first != TOKEN)
        throw logic_error("Error: directive error :" + i->second);
    if ((i + 1) == end || (*(i + 1)).first != COLON)
        throw logic_error("Error: directive error :" + i->second);
    if ((i + 2) == end || ((*(i + 2)).first != TOKEN && (*(i + 2)).first != TOKEN_IN_QUOTES))
        throw logic_error("Error: directive error :" + i->second);
    if (i->second == "host")
    {
        srv.setHostname((*(i + 2)).second);
        i = i + 3;
    }
    else if (i->second == "ports")
        parse_ports(i, end, srv);
    else if (i->second == "body_size")
        parse_body_size(i, srv);
    else if (i->second == "server_names")
        parse_server_names(i, end, srv);
    else if (i->second == "error_pages")
        parse_error_pages(i, end, srv);
    else if (i->second == "location")
    {
        Location lct = parse_location(i, end);
        lct.ready_location();
        srv.addLocation(lct);
    }
    else
        throw logic_error("Error: directive not found :" + i->second);
    if (i == end || i->first != SEMICOLON)
        throw logic_error("Error: directive error :" + i->second);
    return 0;
}

Server  parsing_server(vector<pair<int, string> >::iterator &i, vector<pair<int, string> >::iterator end)
{
    Server srv;
    while (i != end && i->first != CLOSE_BRACKET)
    {
        if (i->first != OPEN_BRACKET)
            parse_directive(i, end, srv);
        i++;
    }
    if (i == end)
        throw logic_error("Error: Bracket not found 1");
    return srv;
}

vector<Server>  parsing(vector<pair<int, string> > tokens)
{
    vector<Server>  res;
    Server          srv;
    for (vector<pair<int, string> >::iterator i = tokens.begin(); i != tokens.end(); i++)
    {
        if (i->first != OPEN_BRACKET)
            throw logic_error("ERROR: server not valid (server only inside brackets)");
        srv = parsing_server(i, tokens.end());
        srv.ready_server();
        res.push_back(srv);
    }
    return res;
}