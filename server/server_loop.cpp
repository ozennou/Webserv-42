// #include <header.hpp>

// struct kevent *init_kevent(int &fd_size, vector<int> &sockets)
// {
//     fd_size = sockets.size();
//     struct kevent   *fdset = new struct kevent[1024];
//     for (int i = 0; i < fd_size; i++)
//         EV_SET(fdset, sockets[i], EVFILT_READ, EV_ADD, 0, 0, NULL); //ffalgs, data, udata
//     return fdset;
// }

// int add_client(struct kevent *fdset, int &fd_max, int &fd_size, int newFd)
// {
//     if (fd_size == fd_max)
//     {
//         try{
//             struct kevent *tmp = new struct kevent[fd_max * 2];
//             fd_max *= 2;
//             for (int i = 0; i < fd_size; i++)
//                 tmp[i] = fdset[i];
//             delete[] fdset;
//             fdset = tmp;
//         } catch(exception &e) {
//             logging("new fail: " + string(e.what()), ERROR, NULL, 0);
//             return 1;
//         }
//     }
//     EV_SET(&fdset[fd_size], newFd, EVFILT_READ | EVFILT_WRITE, EV_ADD, 0, 0, NULL);
//     fd_size++;
//     return 0;
// }

// int new_connection(int &fd, Clients &clients, struct kevent *fdset, int &fd_max, int &fd_size)
// {
//     sockaddr_storage sa;
//     socklen_t t = sizeof(sa);
//     int newFd = accept(fd, (sockaddr*)&sa, &t);
//     if (newFd < 0)
//         return 1;
//     if (sa.ss_family == AF_INET)
//         logging("Accepted ipv4 new connection", INFO, NULL, 0);
//     else
//     {
//         close(newFd);
//         return 1;
//     }
//     clients.add_client(newFd, fd);
//     add_client(fdset, fd_max, fd_size, newFd);
//     return 0;
// }

// int remove_client(Clients &clients, struct kevent *fdset, int &client_fd, int &fd_size)
// {
//     clients.remove_client(client_fd);
//     EV_SET(fdset, client_fd, EVFILT_READ | EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
//     fd_size--;
//     return 0;
// }

// int reading_request(int &client_fd, Clients &clients, struct kevent *fdset, int &fd_size) //add any params you need
// {
//     char    bf[1];
//     int     lenght = recv(client_fd, bf, 1, 0);

//     if (lenght < 0)
//         return 1;
//     else if (!lenght)
//     {
//         logging("Client disconnected", INFO, NULL, 0);
//         remove_client(clients, fdset, client_fd, fd_size);
//     }
//     else
//     {
//         // cout << bf;    // reding the request part
//         (void)bf;
//     }
//     return 0;
// }

// int sending_response(int &client_fd, Socket_map &sock_map, Clients &clients)
// {
//     //you can know servers of the client using
//     int sock_d = clients.get_sock_d(client_fd);
//     if (sock_d < 0)   // used for the client that already desconnected from the same iteration
//         return 1;
//     vector<Server> srv = sock_map.get_servers(sock_d);
//         const char *response =
//         "HTTP/1.1 200 OK\r\n"        
//         "Content-Type: text/html\r\n"
//         "Content-Length: 0\r\n"      
//         "\r\n";                      

//     int result = send(client_fd, response, strlen(response), 0);
//     if (result < 0)
//         return 1;
//     return 0;
// }

// int server_loop(vector<Server> &srvs, Socket_map &sock_map)
// {
//     Clients         clients;
//     vector<int>     sockets = sock_map.get_sockets();
//     int             num_events, fd_max = 1024, fd_size;
//     struct kevent   *fdset = NULL;
//     int             fd, kq = kqueue();

//     (void)srvs;
//     if (kq < 0)
//     {
//         logging("kqueue fail: " + string(strerror(errno)), ERROR, NULL, 0);
//         exit(EXIT_FAILURE);
//     }
//     fdset = init_kevent(fd_size, sockets);
//     while (true)
//     {
//         struct kevent eventlist[FD_SETSIZE];
//         num_events = kevent(kq, fdset, fd_size, eventlist, FD_SETSIZE, NULL);
//         cout << num_events << endl;
//         if (num_events < 0)
//         {
//             logging("kevent fail: " + string(strerror(errno)), ERROR, NULL, 0); //remove it later
//             continue;
//         }
//         for (int i = 0; i < num_events; i++)
//         {
//             fd = eventlist[i].ident;
//             if (eventlist[i].flags & EV_EOF)
//                 cout << "WA333" << endl;
//             if (eventlist[i].filter == EVFILT_READ)
//             {
//                 if (find(sockets.begin(), sockets.end(), fd) != sockets.end())
//                     new_connection(fd, clients, fdset, fd_max, fd_size);
//                 else
//                     reading_request(fd, clients, fdset, fd_size);
//             }
//             else if (eventlist[i].filter == EVFILT_WRITE)
//                 sending_response(fd, sock_map, clients);
//         }
//     }
//     return 0;
// }