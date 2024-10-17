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

#define DEFAULT_CONF "./config_file/default.conf"

using namespace std;

class Server;
class Location;

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
void    init_servers(vector<Server> servers);

#include <Server.hpp>
#include <Location.hpp>
#include <Tokenizer.hpp>
#include <Logs.hpp>
