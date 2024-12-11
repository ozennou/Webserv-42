/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:34:55 by mlouazir          #+#    #+#             */
/*   Updated: 2024/12/10 16:48:28 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Uri.hpp"
#include "../include/RequestParser.hpp"
#include <set>

Uri::Uri( ) {
}

Uri::Uri( const Uri& obj ) {
    *this = obj;
}

Uri& Uri::operator=( const Uri& obj ) {
    if (this != &obj) {
        this->socketFd = obj.socketFd;
        this->socket_map = obj.socket_map;
        this->type = obj.type;
        this->subDelimiters = obj.subDelimiters;
        this->genDelimiters = obj.genDelimiters;
        this->requestTarget = obj.requestTarget;
        this->path = obj.path;
        this->query = obj.query;
        this->host = obj.host;
        this->port = obj.port;
        this->cgi = false;
    }
    return *this;
}

Uri::Uri( int& socketFd, Socket_map& socket_map ) : socketFd(socketFd), socket_map(&socket_map), subDelimiters(SUB_DELIM), genDelimiters(GEN_DELIM) {
    port = 80;
}

Uri::~Uri( ) {
}

void    Uri::reset( ) {
    type = 0;
    requestTarget.clear();
    query.clear();
    host.clear();
    port = 80;
    cgi = false;
}

bool    Uri::isRegularFile( ) {
    struct stat path_stat;

    if (stat(path.c_str(), &path_stat) == -1) return false;
    return S_ISREG(path_stat.st_mode);
}

bool    Uri::isRegularFile( string& pathh ) {
    struct stat path_stat;

    if (stat(pathh.c_str(), &path_stat) == -1) return false;
    return S_ISREG(path_stat.st_mode);
}

bool    Uri::isDirectory( ) {
    struct stat path_stat;

    if (stat(path.c_str(), &path_stat) == -1) return false;
    return S_ISDIR(path_stat.st_mode);
}

void    Uri::checkCGI( Location& location ) {
    if (path.rfind('.') == string::npos) return ;
    
    string extension = path.substr(path.rfind('.'));
    set<string> s = location.getCgiExt();
    set<string>::iterator it = s.begin();

    for (; it != s.end(); it++) {
        if (*(it) == extension) {
            cgi = true;
            break;
        }
    }
    return ;
}

size_t    Uri::getResourceSize( ) {
    struct stat path_stat;

    if (stat(path.c_str(), &path_stat) == -1) RequestParser::HttpRequestException("Stat Failed When getting the size", 500);
    return path_stat.st_size;
}

bool    Uri::getIsCGI( ) {
    return cgi;
}

Location Uri::matchURI( Server& server ) {
    Location location;
    vector<Location> locations = server.getLocations();
    vector<Location>::iterator it = locations.begin();
    
    // Get the route that is at least equal to the path
    for (; (it != locations.end() && it->getRoute().size() > path.size()); it++) {
    }

    if (it == locations.end()) throw RequestParser::HttpRequestException("No Location Was Found", 404);

    string route;
    // Getting the path Of the resource
    for (; it != locations.end(); it++) {
        route = it->getRoute();
        // If the exact mode is ON
        if (it->getExact() && path == route) {
            location = *it;
            break;
        }
        else if (!it->getExact() && (!path.rfind(route, 0)) \
            && (route.length() > location.getRoute().length())) {
            location = *it;
        }
    }
    
    if (!location.getRoute().length()) throw RequestParser::HttpRequestException("No Location Was Found", 404);
    
    path.insert(0, location.getRoot());
    return location;
}

Server Uri::getHostServer( ) {
    vector<Server> servers = socket_map->get_servers(socketFd);

    if (!host.length()) return *(servers.begin());

    for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++) {
        set<string> names = it->getServerNames();
        for (set<string>::iterator itss = names.begin(); itss != names.end(); itss++) {
            if (*itss == host)
                return *(it);
        }
    }
    return *(servers.begin());
}

void Uri::normalizePath( ) {
    size_t j;

    // /./././././././././
    for (size_t i = 0; i < path.length(); i++) {
        if ((path[i] == '.' && (i > 0 && path[i - 1] == '/')) && ((i + 1 == path.length()) || (i + 1 < path.length() && path[i + 1] == '/')))
            path.erase(i, 1);
    }
    
    // /////a/////a//////b////n
    for (size_t i = 0; i < path.length(); i++) {
        j = i;
        for (; j < path.length() && path[j] == '/'; j++) {
        }
        
        if (j != i && path[i] == '/') path.erase(i + 1, (j - (i + 1)));
    }

    // /.. Bad Request /b1/../a1
    for (size_t i = 0; i < path.length(); i++) {
        if ((path[i] == '.' && (i > 0 && path[i - 1] == '/')) \
        && (i + 1 < path.length() && path[i + 1] == '.') \
        && ((i + 2 == path.length()) || (i + 2 < path.length() && path[i + 2] == '/'))) {
            
            if (i == 1 && path[0] == '/') throw RequestParser::HttpRequestException("Invalid Path", 400);

            int a = i - 2;
            for (; a > 0; a--)
                if (path[a] == '/') break;

            if (a == -1) a = 0;
            
            path = path.erase(a + 1, (i + 2) - a);
            i = 0;
        }
    }

    if (!path.length()) throw RequestParser::HttpRequestException("Invalid Path", 400);
}

void Uri::extractQuery( size_t& index ) {
    query = requestTarget.substr(index + 1, requestTarget.length() - (index + 1));
    
    for (size_t i = 0; i < query.length(); i++) {
        if ((!isUnreserved(query[i]) && query[i] != '@' \
            && query[i] != ':' && query[i] != '/' && query[i] != '?') \
        && !percentEncoded(query, i) \
        && subDelimiters.find(query[i]) == string::npos)
            throw RequestParser::HttpRequestException("Invalid Character found in Query", 400);
    }
}

void Uri::originForm( ) {
    size_t i = 1;
    for (; i < requestTarget.length(); i++) {
        if (requestTarget[i] == '?')
            break;
        if ((!isUnreserved(requestTarget[i]) && requestTarget[i] != '@' \
            && requestTarget[i] != ':' && requestTarget[i] != '/') \
        && !percentEncoded(requestTarget, i) \
        && subDelimiters.find(requestTarget[i]) == string::npos)
            throw RequestParser::HttpRequestException("Invalid Character found in path in Origin Form", 400);
    }
    path = requestTarget.substr(0, i);
    if (i != requestTarget.length()) extractQuery(i);
}

void Uri::extractPath( string& requestT ) {
    this->requestTarget = requestT;
    if (requestTarget[0] == '/') {
        originForm();
        type = ORIGIN;
    } else throw RequestParser::HttpRequestException("Invalid Request Target", 400);

    normalizePath();
}