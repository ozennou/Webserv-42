#include <header.hpp>

void    set_hint(struct addrinfo *hints)
{
    memset(hints, 0, sizeof(*hints));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;
    hints->ai_protocol = IPPROTO_TCP;
}

int binding(struct addrinfo *res, int &_sock_d)
{
    int binded = -1, sock_d, opt;
    struct addrinfo *i;
    for (i = res; i != NULL; i = i->ai_next)
    {
        opt = 1;
        sock_d = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (sock_d < 0)
            continue;
        if (setsockopt(sock_d, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int)) < 0)
        {
            close(sock_d);
            continue;
        }
        if (bind(sock_d, i->ai_addr, i->ai_addrlen) < 0)
        {
            close(sock_d);
            continue;
        }
        if (listen(sock_d, SOMAXCONN) < 0)
        {
            close(sock_d);
            continue;
        }
        if (fcntl(sock_d, F_SETFL, O_NONBLOCK) < 0)
        {
            close(sock_d);
            continue;
        }
        binded = 0;
    }
    _sock_d = sock_d;
    return binded;
}

void    init_servers(vector<Server> &servers, Socket_map &sock_map)
{
    int status, init = 0, sock_d;
    string      host;
    Socket_map  sock;
    for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        set<int>    ports = it->getPorts();
        host = it->getHostname();
        for (set<int>::iterator port = ports.begin(); port != ports.end(); port++)
        {
            struct addrinfo hints, *res;
            set_hint(&hints);
            status = getaddrinfo(host.c_str(), to_string(*port).c_str(), &hints, &res);
            if (status != 0)
            {
                logging("getaddrinfo fail: " + string(gai_strerror(status)), ERROR, &(*it), *port);
                continue ; 
            }
            if (binding(res, sock_d) < 0)
            {
                if (sock_map.exist_hp(host + ":" + to_string(*port)))
                {
                    sock_map.add_server(host + ":" + to_string(*port), *it);
                    logging("Initialized successfully.", INFO, &(*it), *port);
                }
                else
                    logging("initializing fail: " + string(strerror(errno)), ERROR, &(*it), *port);
                freeaddrinfo(res);
                continue ;
            }
            freeaddrinfo(res);
            init++;
            logging("Initialized successfully.", INFO, &(*it), *port);
            sock_map.add_sock(host + ":" + to_string(*port), sock_d, *it);
        }
    }
    if (!servers.size())
        throw logic_error("Error: empty server");
    if (!init)
        throw logic_error("Error: no server initialized succefully");
}