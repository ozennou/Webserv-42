#pragma once

#include "header.hpp"

class Clients {
    private:
        map<int /*client_d  */, int /* sock_d */> client_map; 
    public:
        Clients();
        ~Clients();

        map<int, int>   get_map(void);
        void            add_client(int cliend_d, int sock_d);
        int             exist_client(int client_d);
        int             get_sock_d(int client_d);
        void            remove_client(int client_d);
        void            clients_loop(Socket_map &sock_map);
};