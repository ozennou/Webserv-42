/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   header.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mozennou <mozennou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/14 15:57:47 by mozennou          #+#    #+#             */
/*   Updated: 2024/10/14 22:20:30 by mozennou         ###   ########.fr       */
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

#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define RESET   "\033[0m"

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
#include <Server.hpp>
#include <Location.hpp>
#include <Tokenizer.hpp>
