#include <header.hpp>
#include "../include/Bond.hpp"

vector<Bond>::iterator getBond( vector<Bond>& bonds, int clientFd ) {
    vector<Bond>::iterator it = bonds.begin();
    for (; it != bonds.end(); it++) {
        if ((it)->getClientFd() == clientFd) return it;
    }
    it = bonds.end();
    return it;
}

struct pollfd *init_poll_struct(vector<int> sockets, int &size, int &max_size)
{
    size = sockets.size();
    while (max_size < size)
        max_size *= 2;
    struct pollfd *pfds = new struct pollfd[max_size];
    for (unsigned long i = 0; i < sockets.size(); i++)
    {
        pfds[i].fd = sockets[i];
        pfds[i].events = POLLIN;
        pfds[i].revents = 0;
    }
    return pfds;
}

void add_client(struct pollfd **pfds, int &newFd, int &size, int &max_size)
{
    int i;
    for (i = 0; i < size; i++)
    {
        if ((*pfds)[i].fd == -1)
            break;
    }
    if (i < size)
    {
        (*pfds)[i].fd = newFd;
        (*pfds)[i].events = POLLIN | POLLOUT;
        (*pfds)[i].revents = 0;
    }
    else
    {
        if (size >= max_size)
        {
            struct pollfd *tmp = new struct pollfd[max_size * 2];
            for (int i = 0; i < size; i++)
                tmp[i] = (*pfds)[i];
            delete[] (*pfds);
            max_size *= 2;
            (*pfds) = tmp;
        }
        (*pfds)[size].fd = newFd;
        (*pfds)[size].events = POLLIN | POLLOUT;
        (*pfds)[size++].revents = 0;
    }
}

int newconnection2(Clients &clients, vector<Bond> &bonds, int &fd, struct pollfd **pfds, int &size, int &max_size)
{
    sockaddr_storage sa;
    socklen_t t = sizeof(sa);
    int newFd = accept(fd, (sockaddr*)&sa, &t);
    if (newFd < 0)
        return 1;
    if (sa.ss_family == AF_INET) {
        logging("Accepted ipv4 new connection", INFO, NULL, 0);
    }
    else
    {
        close(newFd);
        return 1;
    }
    add_client(pfds, newFd, size, max_size);
    clients.add_client(newFd, fd);
    bonds.push_back(newFd);
    return 0;
}

int reading_request2(int &client_fd, Clients &clients, vector<Bond> &bonds, struct pollfd *pfds, int &i)
{
    vector<Bond>::iterator    bond = getBond(bonds, client_fd);

    if (bond == bonds.end()) return 1;

        try {
            bond->initParcer();
        }
        catch(const RequestParser::HttpRequestException& e) {
            if (e.statusCode == 0) {
                logging("Client Disconnected", ERROR, NULL, 0);
                pfds[i].fd = -1;
                clients.remove_client(i);
                bonds.erase(bond);
                close(client_fd);
            }
            else if (e.statusCode == -1)
                return 1;
            else
                logging(e.what(), ERROR, NULL, 0);
        }
    return 1;
}

int sending_response2(Clients &clients, vector<Bond> &bonds, int &client_fd, Socket_map &sock_map)
{
    int sock_d = clients.get_sock_d(client_fd);
    if (sock_d < 0)
        return 1;
    vector<Bond>::iterator    bond = getBond(bonds, client_fd);
    if (bond->getRequestState() == PROCESSING) return 0;
    bond->initBuilder();

    return 0;
}

int poll_loop(vector<Server> &srvs, Socket_map &sock_map)
{
    vector<Bond>  bonds;
    Clients       clients;
    int           size, fd, max_size = 1;
    vector<int> sockets = sock_map.get_sockets();
    struct pollfd *pfds = init_poll_struct(sockets, size, max_size);

    bonds.reserve(size);
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
                fd = pfds[i].fd;
                if (fd < 0)
                    continue;
                if (pfds[i].revents & POLLIN)
                {
                    if (find(sockets.begin(), sockets.end(), fd) != sockets.end())
                        newconnection2(clients, bonds, fd, &pfds, size, max_size);
                    else
                        reading_request2(fd, clients, bonds, pfds, i);
                } else if (pfds[i].revents & POLLOUT)
                {
                    sending_response2(clients, bonds, fd, sock_map);
                }

            }
        }
    }
}