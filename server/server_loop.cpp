#include <header.hpp>


void    server_loop(vector<Server> &srvs, Socket_map &sock_map)
{
    poll_loop(srvs, sock_map);
}