#pragma once

#include <header.hpp>

typedef map<string, pair<int, vector<Server> > > map_sock;

class Server;

class Socket_map {
    private:
        map_sock sock_map;
    public:
        Socket_map();
        ~Socket_map();

        void            add_sock(const string &hp, int &sock_d, Server &srv);
        int             exist_hp(const string &hp);
        void            add_server(const string &hp, Server &srv);
        vector<Server>  get_servers(const string &hp);
        vector<Server>  get_servers(int &sock_d);
        vector<int>     get_sockets(void);
        void print_map(void);
} ;