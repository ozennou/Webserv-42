#include <header.hpp>

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

int newconnection2(Clients &clients, int &fd, struct pollfd **pfds, int &size, int &max_size)
{
    sockaddr_storage sa;
    socklen_t t = sizeof(sa);
    int newFd = accept(fd, (sockaddr*)&sa, &t);
    if (newFd < 0)
        return 1;
    if (sa.ss_family == AF_INET)
        logging("Accepted ipv4 new connection", INFO, NULL, 0);
    else
    {
        close(newFd);
        return 1;
    }
    if (add_client(pfds, newFd, size, max_size))
        return 1;
    clients.add_client(newFd, fd);
    return 0;
}

int reading_request2(int &client_fd, Clients &clients, struct pollfd *pfds, int &i)
{
    char    bf[1024];
    int     lenght = recv(client_fd, bf, 1024, 0);

    if (lenght < 0)
        return 1;
    else if (!lenght)
    {
        logging("Client disconnected", INFO, NULL, 0);
        pfds[i].fd = -1;
        clients.remove_client(i);
        close(client_fd);
    }
    else
    {
        // cout << bf << endl;;    // reding the request part
        (void)bf;
    }
    return 0;
}

int sending_response2(int &client_fd, Clients &clients, Socket_map &sock_map)
{
    int sock_d = clients.get_sock_d(client_fd);

        const char *response =
        "HTTP/1.1 200 OK\r\n"        
        "Content-Type: text/html\r\n"
        "Content-Length: 2\r\n"      
        "\r\n"
        "AH\r\n"      
        ;

    int result = send(client_fd, response, strlen(response), 0);
    if (result < 0)
        return 1;
    return 0;
    vector<Server> srv = sock_map.get_servers(sock_d);
    if (sock_d < 0)   // used for the client that already desconnected from the same iteration
        return 1;
}

int poll_loop(vector<Server> &srvs, Socket_map &sock_map)
{
    (void)srvs;
    Clients       clients;
    int           size, fd, max_size = 1;
    vector<int> sockets = sock_map.get_sockets();
    struct pollfd *pfds = init_poll_struct(sockets, size, max_size);

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
                        newconnection2(clients, fd, &pfds, size, max_size);
                    else
                    {
                        reading_request2(fd, clients, pfds, i);
                    }
                } else if (pfds[i].revents & POLLOUT)
                {
                    sending_response2(fd, clients, sock_map);
                    pfds[i].events = POLLIN;
                }

            }
        }
    }
}