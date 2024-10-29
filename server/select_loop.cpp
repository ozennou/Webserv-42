#include "../include/RequestParser.hpp"

int init_set(fd_set &fdset, vector<int> &sockets)
{
    int fd_max = -1;
    FD_ZERO(&fdset);
    for (vector<int>::iterator i = sockets.begin(); i != sockets.end(); i++)
    {
        FD_SET(*i, &fdset);
        if (*i > fd_max)
            fd_max = *i;
    }
    return fd_max;
}

void add_client(Clients &clients, fd_set &fdset, int &fd_max, int &newFd, int &i)
{
    clients.add_client(newFd, i);
    FD_SET(newFd, &fdset);
    if (newFd > fd_max)
        fd_max = newFd;
}

void remove_client(Clients &clients, fd_set &fdset, int &i)
{
    clients.remove_client(i);
    close(i);
    FD_CLR(i, &fdset);
}

int new_connection(int &i, Clients &clients, fd_set &fdset, int &fd_max)
{
    sockaddr_storage sa;
    socklen_t t = sizeof(sa);
    int newFd = accept(i, (sockaddr*)&sa, &t);
    if (newFd < 0)
        return 1;
    if (sa.ss_family == AF_INET)
        logging("Accepted ipv4 new connection", INFO, NULL, 0);
    else
    {
        close(newFd);
        return 1;
    }
    add_client(clients, fdset, fd_max, newFd, i);
    return 0;
}

int reading_request(int &client_fd, Clients &clients, fd_set &fdset) //add any params you need
{
    RequestParser requestParser(client_fd);
    try {

        requestParser.fillRequestObject();
    } catch( RequestParser::HttpRequestException &e ) {
        cout << e.message << ' ' << e.statusCode << endl; 
        if (e.statusCode < 0)
            return 1;
        else if (!e.statusCode)
        {
            logging("Client disconnected", INFO, NULL, 0);
            remove_client(clients, fdset, client_fd);
        }
    }
    return 0;
}

int sending_response(int &client_fd, Socket_map &sock_map, Clients &clients)
{
    //you can know servers of the client using
    int sock_d = clients.get_sock_d(client_fd);
    if (sock_d < 0)   // used for the client that already desconnected from the same iteration
        return 1;
    vector<Server> srv = sock_map.get_servers(sock_d);
    return 0;
}

int server_loop(vector<Server> &srvs, Socket_map &sock_map)
{
    Clients     clients;
    fd_set      fdset;
    vector<int> sockets = sock_map.get_sockets();
    int         fd_max = init_set(fdset, sockets);
    (void)srvs;

    while (true)
    {
        fd_set readfd = fdset;
        fd_set writefd = fdset;
        if (fd_max >= 1024)
            fd_max = 1023;
        if (select(fd_max + 1, &readfd, &writefd, NULL, NULL) < 0)
            continue;
        for (int i = 0; i <= fd_max; i++)
        {
            if (FD_ISSET(i, &readfd))
            {
                if (find(sockets.begin(), sockets.end(), i) != sockets.end())
                {
                    if (new_connection(i, clients, fdset, fd_max))
                        continue;
                }
                else
                {
                    if (reading_request(i, clients, fdset))
                        continue ;
                }
            }
            if (FD_ISSET(i, &writefd))
            {
                if (sending_response(i, sock_map, clients))
                    continue;
            }
        }
    }
    return 0;
}