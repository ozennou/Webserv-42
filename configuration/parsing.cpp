#include "header.hpp"

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
        if (ss.fail() || !ss.eof() || error_code < 400 || error_code > 599)
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

void    parse_mime_types_file(vector<pair<int, string> >::iterator &i, Server &srv)
{
    i += 2;
    ifstream mime_file(i->second);
    vector<pair<int, string> > tokens;
    if (!mime_file.is_open())
        throw logic_error("Error: opening mime types file: " + i->second);
    tokens = tokenizer(mime_file);
    mime_file.close();
    vector<pair<int, string> >::iterator end = tokens.end();
    for (vector<pair<int, string> >::iterator it = tokens.begin(); it != end; it++)
    {
        string ext, type;
        if (it == end || (it->first != TOKEN && it->first != TOKEN_IN_QUOTES))
            throw logic_error("Error: mime type format 'ext: type;'1");
        ext = (it++)->second;
        if (it == end || it->first != COLON)
            throw logic_error("Error: mime type format 'ext: type;'2");
        if ((++it) == end || (it->first != TOKEN && it->first != TOKEN_IN_QUOTES))
            throw logic_error("Error: mime type format 'ext: type;'3");
        type = (it++)->second;
        if (it == end || it->first != SEMICOLON)
            throw logic_error("Error: mime type format 'ext: type;'4");
        srv.setMimeType(ext, type);
    }
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
    else if (i->second == "mime_types")
    {
        parse_mime_types_file(i, srv);
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
    int             index = 1;
    for (vector<pair<int, string> >::iterator i = tokens.begin(); i != tokens.end(); i++)
    {
        if (i->first != OPEN_BRACKET)
            throw logic_error("ERROR: server not valid (server only inside brackets)");
        srv = parsing_server(i, tokens.end());
        srv.ready_server();
        srv.set_index(index++);
        srv.sort_location();
        res.push_back(srv);
    }
    return res;
}