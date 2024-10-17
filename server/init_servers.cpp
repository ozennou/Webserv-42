#include <header.hpp>

void    set_hint(struct addrinfo *hints)
{
    memset(hints, 0, sizeof(*hints));
    hints->ai_family = AF_INET;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags = AI_PASSIVE;
    hints->ai_protocol = IPPROTO_TCP;
}

void    init_servers(vector<Server> servers)
{
    int status;
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
                logging(gai_strerror(status), ERROR, &(*it), *port);
                continue ; 
            }

        }
    }
}