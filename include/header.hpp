#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <list>
#include <vector>
#include <set>
#include <map>
#include <ctime>
#include <utility>
#include <algorithm>
#include <unistd.h>
#include <pwd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <poll.h>
#include <arpa/inet.h>
#include <fstream>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

// #include "IBond.hpp"

#define DEFAULT_CONF "./config_file/default.conf"
#define POLL_TIMEOUT 10000
#define LOGS true

#define CRLF "\r\n"
#define GEN_DELIM ":/?#[]@"
#define SUB_DELIM "!$&'()*+,;="
#define DELI "\"(),/:;<=>?@[\\]{}"

enum {
    OPEN,
    CLOSED,
    REQUEST_READY,
    RESPONSE_READY,
    INT_RANGE,
    SUFFIX_RANGE,
    NO_RANGE,
    UPLOADING,
    UPLOADED,
    CHUNKED_LENGTH,
    CHUNKED_DATA,
    BOUNDARY,
    HEADERS
};

using namespace std;

class Server;
class Location;
class Socket_map;
class Clients;

//printf_server.cpp
void print_servers(vector<Server> &);

void stolower( string& s );

bool isUnreserved( int c );

bool percentEncoded( string& str, size_t index );

//check_config_file.cpp
vector<Server>    check_config_file(string path);

//tokenizer.cpp
vector<pair<int, string> >  tokenizer(ifstream& conf_file);

//parsing.cpp
vector<Server>  parsing(vector<pair<int, string> > tokens);

//parse_location.cpp
Location    parse_location(vector<pair<int, string> >::iterator &i, vector<pair<int, string> >::iterator end);

//Logs.cpp
void    logging(const string& msg, int level, Server *srv, int port);
string  logging_file(const char *input, int opt);
int     set_value(int i);

//init_servers.cpp
void    init_servers(vector<Server> &servers, Socket_map &sock_map);

//check_flags.cpp
string check_flags(int ac, char **av);

size_t  getRangeValue( string rangeString );


int poll_loop(Socket_map &sock_map);
struct pollfd *init_poll_struct(vector<int> sockets, int &size);

//Cgi.cpp
string CheckExec(string cgiExec);
string getCgiExec(string cgiExt);
void delete_envs(char **envs, int *fd);


#include <Server.hpp>
#include <Location.hpp>
#include <Tokenizer.hpp>
#include <Logs.hpp>
#include <Socket_map.hpp>
#include <Clients.hpp>