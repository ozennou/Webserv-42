/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Uri.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mlouazir <mlouazir@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/23 21:34:55 by mlouazir          #+#    #+#             */
/*   Updated: 2024/11/12 15:53:16 by mlouazir         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Uri.hpp"
#include "../include/RequestParser.hpp"
#include <set>

Uri::Uri( int socketFd, Socket_map& socket_map ) : socketFd(socketFd), socket_map(&socket_map), subDelimiters(SUB_DELIM), genDelimiters(GEN_DELIM) {
    port = 80;
}

// Uri& Uri::operator=( Uri& obj ) {
//     if (this != &obj) {
//         this->socketFd = obj.socketFd;
//         this->socket_map = obj.socket_map;
//         this->type = obj.type;
//         this->subDelimiters = obj.subDelimiters;
//         this->genDelimiters = obj.genDelimiters;
//         this->requestTarget = obj.requestTarget;
//         this->path = obj.path;
//         this->query = obj.query;
//         this->host = obj.host;
//         this->port = obj.port;
//     }
//     return *this;
// }

Uri::~Uri( ) {
}

bool    Uri::isRegularFile( ) {
    struct stat path_stat;

    if (stat(path.c_str(), &path_stat) == -1) return false;
    return S_ISREG(path_stat.st_mode);
}

bool    Uri::isDirectory( ) {
    struct stat path_stat;

    if (stat(path.c_str(), &path_stat) == -1) return false;
    return S_ISDIR(path_stat.st_mode);
}

Location Uri::matchURI( Server& server ) {
    Location location;
    vector<Location> locations = server.getLocations();
    vector<Location>::iterator it = locations.begin();

    // Get the route that is at least equal to the path
    for (; it->getRoute().size() > path.size(); it++) {
    }

    // Getting the path Of the resource
    for (; it != locations.end(); it++) {
        // If the exact mode is ON
        if (it->getExact() && path == it->getRoute()) {
            location = *it;
            break;
        }
        else if (!it->getExact() && (!path.rfind(it->getRoute(), 0)) \
            && (it->getRoute().length() > location.getRoute().length())) {
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

void Uri::extractQuery( size_t index ) {
    query = requestTarget.substr(index + 1, requestTarget.length() - (index + 1));
    
    for (size_t i = 0; i < requestTarget.length(); i++) {
        if ((!isUnreserved(requestTarget[i]) && requestTarget[i] != '@' \
            && requestTarget[i] != ':' && requestTarget[i] != '/' && requestTarget[i] != '?') \
        && !percentEncoded(requestTarget, i) \
        && subDelimiters.find(requestTarget[i]) == string::npos)
            throw RequestParser::HttpRequestException("Invalid Character found in Query", 400);
    }
}

void Uri::absoluteForm( ) {
    size_t colonIndex = requestTarget.find(':');

    if (colonIndex == string::npos) throw RequestParser::HttpRequestException("':' not found in absolute-fome URI", 400);
    if (colonIndex != 4 || requestTarget.compare(0, 4, "http")) throw RequestParser::HttpRequestException("Invalid scheme received for the absolut-form", 400);

    size_t i = 4;

    if ((i + 2 >= requestTarget.length()) \
    || requestTarget[i + 1] != '/' \
    || requestTarget[i + 2] != '/')
        throw RequestParser::HttpRequestException("No // found after the scheme", 400);
    
    i += 3;

    for (; i < requestTarget.length(); i++) {
        if (genDelimiters.find(requestTarget[i]) != string::npos)
            break;
        if (!isUnreserved(requestTarget[i]) \
        && (!percentEncoded(requestTarget, i)) \
        && subDelimiters.find(requestTarget[i]) == string::npos)
            throw RequestParser::HttpRequestException("Invalid Character found in path in Host Absolut Form", 400);
    }

    host = requestTarget.substr(7, (i -  (7)));

    if (!host.length()) throw RequestParser::HttpRequestException("No Host Found in absolut-form", 400);

    if (i == requestTarget.length()) return ;

    if ((requestTarget[i] != '/') && (requestTarget[i] != '?') && (requestTarget[i] != ':'))
        throw RequestParser::HttpRequestException("Invalid delimiter after host", 400);
    
    if (requestTarget[i] == ':') {
        size_t j = i + 1;
        for (; j < requestTarget.length(); j++) {
            if (genDelimiters.find(requestTarget[j]) != string::npos)
                break;
            if (!isdigit(requestTarget[j]))
                throw RequestParser::HttpRequestException("Invalid Character In the Port Number", 400);
        }
        if (j != i + 1) {
            stringstream ss;
            ss << requestTarget.substr(i + 1, (j - (i + 1))); ss >> port;
            if (ss.fail() || port < 0 || port > 65535) throw RequestParser::HttpRequestException("Invalid Port Number", 400);
        }

        i = j;
        if (i == requestTarget.length()) return;

        if (requestTarget[i] != '/' && requestTarget[i] != '?')
            throw RequestParser::HttpRequestException("Invalid delimiter after port", 400);

    }

    if (requestTarget[i] == '/') {
        size_t j = i + 1;
        for (; j < requestTarget.length(); j++) {
            if (genDelimiters.find(requestTarget[j]) != string::npos \
                && requestTarget[j] != '/')
                break;
            if ((!isUnreserved(requestTarget[j]) && requestTarget[j] != '@' && requestTarget[j] != ':' \
                && requestTarget[j] != '/') \
            && (!percentEncoded(requestTarget, j)) \
            && subDelimiters.find(requestTarget[j]) == string::npos)
                throw RequestParser::HttpRequestException("Invalid character found in path", 400);
        }

        path = requestTarget.substr(i, j - i);

        i = j;
        if (i == requestTarget.length()) return;

        if (requestTarget[i] != '?') throw RequestParser::HttpRequestException("Invalid character after path", 400);
    }

    if (requestTarget[i] == '?') extractQuery(i);
}

void Uri::originForm( ) {
    size_t i = 1;
    for (; i < requestTarget.length(); i++) {
        if ((!isUnreserved(requestTarget[i]) && requestTarget[i] != '@' \
            && requestTarget[i] != ':' && requestTarget[i] != '/') \
        && !percentEncoded(requestTarget, i) \
        && subDelimiters.find(requestTarget[i]) == string::npos)
            throw RequestParser::HttpRequestException("Invalid Character found in path in Origin Form", 400);
        if (requestTarget[i] == '?')
            break;
    }
    path = requestTarget.substr(0, i);
    if (i != requestTarget.length()) extractQuery(i);
}

void Uri::extractPath( string requestT ) {
    this->requestTarget = requestT;
    if (requestTarget[0] == '/') {
        originForm();
        type = ORIGIN;
    }
    else {
        absoluteForm();
        type = ABSOLUTE;
        if (!path.length()) path = "/";
    }

    normalizePath();
}