#include <header.hpp>

void    set_hint(struct addrinfo *hints)
{
    memset(hints, 0, sizeof(*hints));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;
    hints->ai_protocol = IPPROTO_TCP;
}

int binding(struct addrinfo *res)
{
    int binded = -1, sock_d;
    for (struct addrinfo *i = res; i != NULL; i = i->ai_next)
    {
        sock_d = socket(i->ai_family, i->ai_socktype, i->ai_protocol);
        if (sock_d < 0)
            continue;
        if (bind(sock_d, i->ai_addr, i->ai_addrlen) < 0)
            continue;


        binded = 0;
    }
    return binded;
}

void    init_servers(vector<Server> servers)
{
    int status, init = 0;
    for (vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
    {
        set<int>    ports = it->getPorts();
        for (set<int>::iterator port = ports.begin(); port != ports.end(); port++)
        {
            struct addrinfo hints, *res;
            set_hint(&hints);
            status = getaddrinfo(it->getHostname().c_str(), to_string(*port).c_str(), &hints, &res);
            if (status != 0)
            {
                logging("getaddrinfo fail: " + string(gai_strerror(status)), ERROR, &(*it), *port);
                continue ; 
            }
            if (binding(res) < 0)
            {
                logging("binding fail: " + string(strerror(errno)), ERROR, &(*it), *port);
                continue ;
            }
            freeaddrinfo(res);
            init++;
        }
    }
    if (!init)
        throw logic_error("Error: no server initialized succefully");
}