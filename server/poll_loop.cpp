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

void poll_loop(vector<Server> &srvs, Socket_map &sock_map)
{
    (void)srvs; //remove
    int size = 0;
    struct pollfd *pfds = init_poll_struct(sock_map.get_sockets(), size);
    while (true)
    {
        int f = poll(pfds, size, POLL_TIMEOUT);
        if (f < 0)
        {
            logging("poll fail: " + string(strerror(errno)), ERROR, NULL, 0);
            continue;
        }
        else if (!f)
            continue;
        else
        {
            for (int i = 0; i < size; i++)
            {
                if (pfds[i].revents & POLLIN)
                {
                    char bf[1024];
                    sockaddr_storage res;
                    socklen_t t = sizeof(res);
                    int a = accept(pfds[i].fd, (sockaddr*)&res, &t);
                    if (res.ss_family == AF_INET)
                    {
                        sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(&res);
                        char ip[INET_ADDRSTRLEN];
                        inet_ntop(AF_INET, &addr->sin_addr, ip, sizeof(ip));
                        logging("New accepted connection: " + string(ip) + ":" + to_string(ntohs(addr->sin_port)), INFO, NULL, 0);
                    }
                    // recv(a, bf, 1024, 0);
                    // cout << bf;
                    // cout.flush();
                    (void)bf;
                    (void)a;
                    // close(a);
                }
                else if (pfds[i].revents & POLLOUT)
                {
                    cout << "HERE: " << i << endl;
                }
            }
        }
    }
    delete pfds;
}