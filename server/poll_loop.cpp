#include <header.hpp>

struct pollfd *init_poll_struct(vector<int> sockets, int &size)
{
    size = sockets.size();
    struct pollfd *pfds = new struct pollfd[size];
    for (unsigned long i = 0; i < sockets.size(); i++)
    {
        pfds[i].fd = sockets[i];
        pfds[i].events = POLLIN | POLLOUT;
        pfds[i].revents = 0;
    }
    return pfds;
}

void check_event(struct pollfd *pfds, int size, Clients& clients){
    for (int i = 0; i < size; i++)
    {
        if (pfds[i].revents & POLLIN)
        {
            sockaddr_storage res;
            socklen_t t = sizeof(res);
            int a = accept(pfds[i].fd, (sockaddr*)&res, &t);
            if (a < 0)
                continue;
            if (res.ss_family == AF_INET)
            {
                sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&res);
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
                logging("accepted ipv4 connection: " + string(ip) + ":" + to_string(ntohs(addr->sin_port)), INFO, NULL, 0);
            } else if (res.ss_family == AF_INET6)
            {
                sockaddr_in6* addr = reinterpret_cast<sockaddr_in6*>(&res);
                char ip[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, &addr->sin6_addr, ip, sizeof(ip));
                logging("accepted ipv6 connection: " + string(ip) + ":" + to_string(ntohs(addr->sin6_port)), INFO, NULL, 0);
            }
            else
            {
                close(a);
                continue;
            }
            clients.add_client(a, pfds[i].fd);
        }
        if (pfds[i].revents & POLLOUT)
        {
            cout << "HERE: " << i << endl;
        }
    }
}

int poll_loop(struct pollfd *pfds, Clients& clients, int &size)
{
    int f = poll(pfds, size, POLL_TIMEOUT);
    if (f < 0)
    {
        logging("poll fail: " + string(strerror(errno)), ERROR, NULL, 0);
        return 1;
    }
    else if (!f)
        return 1;
    else
        check_event(pfds, size, clients);
    return 0;
}