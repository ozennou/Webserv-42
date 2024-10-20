#include <header.hpp>

void    server_loop(vector<Server> &srvs, Socket_map &sock_map)
{
    Clients clients;
    int size = 0;
    struct pollfd *pfds = init_poll_struct(sock_map.get_sockets(), size);
    (void)srvs;
    while (true)
    {
        if (poll_loop(pfds, clients, size))
            continue;
        clients.clients_loop(sock_map);
    }
}