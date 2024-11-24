#include "header.hpp"
#include "../include/Bond.hpp"

list<Bond>::iterator getBond( list<Bond>& bonds, int clientFd ) {
    list<Bond>::iterator it = bonds.begin();
    for (; it != bonds.end(); it++) {
        if ((it)->getClientFd() == clientFd) return it;
    }
    it = bonds.end();
    return it;
}

void    close_all_fd(vector<int> fds)
{
    for (vector<int>::iterator i = fds.begin(); i != fds.end(); i++)
        close(*i);
}

struct pollfd *init_poll_struct(vector<int> sockets, int &size, int &max_size)
{
    size = sockets.size();
    while (max_size < size)
        max_size *= 2;
    struct pollfd *pfds = new(nothrow) struct pollfd[max_size];
    if (!pfds)
    {
        close_all_fd(sockets);
        throw bad_alloc();
    }
    for (unsigned long i = 0; i < sockets.size(); i++)
    {
        pfds[i].fd = sockets[i];
        pfds[i].events = POLLIN;
        pfds[i].revents = 0;
    }
    return pfds;
}

int add_client(struct pollfd **pfds, int &newFd, int &size, int &max_size)
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
            struct pollfd *tmp = new(nothrow) struct pollfd[max_size * 2];
            if (!tmp)
                return 1;
            for (int j = 0; j < size; j++)
                tmp[j] = (*pfds)[j];
            delete[] (*pfds);
            max_size *= 2;
            (*pfds) = tmp;
        }
        (*pfds)[size].fd = newFd;
        (*pfds)[size].events = POLLIN | POLLOUT;
        (*pfds)[size++].revents = 0;
    }
    return 0;
}

int newconnection2(Clients &clients, list<Bond> &bonds, map<int, string> &statusCodeMap, int &fd, struct pollfd **pfds, int &size, int &max_size, Socket_map& socket_map)
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
    if (add_client(pfds, newFd, size, max_size))
        return 1;
    clients.add_client(newFd, fd);
    Bond b(newFd, fd, socket_map, statusCodeMap); // The Object is constructed in the function's stack
    bonds.push_back(b); // The list's object is constructed in heap by it COPY constructor
    return 0;
}

int reading_request2(int &client_fd, Clients &clients, list<Bond> &bonds,struct pollfd *pfds, int &i)
{

    list<Bond>::iterator  bond = getBond(bonds, client_fd); // Getting The Iterator
    
    if (bond == bonds.end()) return 1;

    try {
        bond->initParcer();
    }
    catch(const RequestParser::HttpRequestException& e) {
        if (e.statusCode == 0) {
            logging("Client Disconnected", ERROR, NULL, 0);
            bonds.erase(bond);
            pfds[i].fd = -1;
            clients.remove_client(client_fd);
            close(client_fd);
        }
        else if (e.statusCode == -1) {
            return 1;
        }
        else
            logging(e.message, ERROR, NULL, 0);
    }
    return 1;
}

int sending_response2(Clients &clients, list<Bond> &bonds, struct pollfd *pfds, int &client_fd, int &i)
{
    list<Bond>::iterator bond = getBond(bonds, client_fd);

    if (bond == bonds.end()) return 1;

    bond->initResponse();
    
    if (!bond->getConnectionState() && bond->getResponseState() == CLOSED) {
        logging("Client Disconnected", ERROR, NULL, 0);
        bonds.erase(bond);
        pfds[i].fd = -1;
        clients.remove_client(client_fd);
        close(client_fd);
    }
    return 0;
}

int poll_loop(vector<Server> &srvs, Socket_map &sock_map)
{
    (void)srvs;
    map<int, string> statusCodeMap;

    statusCodeMap.insert(make_pair<int, string>(505, " HTTP Version Not Supported"));
    statusCodeMap.insert(make_pair<int, string>(501, " Not Implemented"));
    statusCodeMap.insert(make_pair<int, string>(500, " Internal Server Error"));
    statusCodeMap.insert(make_pair<int, string>(416, " Range Not Satisfiable"));
    statusCodeMap.insert(make_pair<int, string>(405, " Method Not Allowed"));
    statusCodeMap.insert(make_pair<int, string>(404, " Not Found"));
    statusCodeMap.insert(make_pair<int, string>(403, " Forbidden"));
    statusCodeMap.insert(make_pair<int, string>(400, " Bad Request"));
    statusCodeMap.insert(make_pair<int, string>(206, " Partial Content"));
    statusCodeMap.insert(make_pair<int, string>(200, " OK"));

    list<Bond>      bonds;
    Clients         clients;
    int             size, fd, max_size = 1;
    vector<int>     sockets = sock_map.get_sockets();
    struct pollfd   *pfds = init_poll_struct(sockets, size, max_size);

    while (true)
    {
        if (set_value(-1))
        {
            for (int i = 0; i < size; i++)
                close(pfds[i].fd);
            delete[] pfds;
            exit(0);
        }
        int f = poll(pfds, size, POLL_TIMEOUT);
        if (f < 0)
            continue;
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
                        newconnection2(clients, bonds, statusCodeMap, fd, &pfds, size, max_size, sock_map);
                    else
                        reading_request2(fd, clients, bonds, pfds, i);
                } else if (pfds[i].revents & POLLOUT) {
                    sending_response2(clients, bonds, pfds, fd, i);
                }
            }
        }
    }
}