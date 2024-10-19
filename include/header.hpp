/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mozennou <mozennou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:57:47 by mozennou          #+#    #+#             */
/*   Updated: 2024/10/16 16:16:46 by mozennou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <algorithm>
#include <unistd.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <poll.h>

#define DEFAULT_CONF "./config_file/default.conf"
#define BACKLOG 30
#define POLL_TIMEOUT 10000

using namespace std;

class Server;
class Location;
class Socket_map;

//printf_server.cpp
void print_servers(vector<Server> &);

//check_config_file.cpp
vector<Server>    check_config_file(int ac, char **av);

//tokenizer.cpp
vector<pair<int, string> >  tokenizer(ifstream& conf_file);

//parsing.cpp
vector<Server>  parsing(vector<pair<int, string> > tokens);

//parse_location.cpp
Location    parse_location(vector<pair<int, string> >::iterator &i, vector<pair<int, string> >::iterator end);

//Logs.cpp
void    logging(const string& msg, int level, Server *srv, int port);

//init_servers.cpp
void    init_servers(vector<Server> &servers, Socket_map &sock_map);

//server_loop.cpp
void    server_loop(vector<Server> &srvs, Socket_map &sock_map);

//to_remove
void poll_loop(vector<Server> &srvs, Socket_map &sock_map);
//end_remove

#include <Server.hpp>
#include <Location.hpp>
#include <Tokenizer.hpp>
#include <Logs.hpp>
#include <Socket_map.hpp>