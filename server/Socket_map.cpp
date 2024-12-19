#include "header.hpp"

Socket_map::Socket_map()
{
}

Socket_map::~Socket_map()
{
}

void Socket_map::add_sock(const string &hp, int &sock_d, Server &srv)
{
    vector<Server> srvs;

    srvs.push_back(srv);
    sock_map.insert(make_pair(hp, make_pair(sock_d, srvs)));
}

int Socket_map::exist_hp(const string &hp)
{
    map_sock::iterator i = sock_map.find(hp);
    if (i == sock_map.end())
        return 0;
    return 1;
}

void Socket_map::add_server(const string &hp, Server &srv)
{
    map_sock::iterator i = sock_map.find(hp);
    if (i == sock_map.end())
    {
        logging("Host:Port not found in the sockets map: ", WARNING, NULL, 0);
        return ;
    }
    i->second.second.push_back(srv);
}

vector<Server> Socket_map::get_servers(const string &hp)
{
    map_sock::iterator i = sock_map.find(hp);
    if (i == sock_map.end())
    {
        logging("Host:Port not found in the sockets map: ", WARNING, NULL, 0);
        return vector<Server>();
    }
    return i->second.second;
}

vector<Server> Socket_map::get_servers(int sock_d)
{
    for (map_sock::iterator i = sock_map.begin(); i != sock_map.end(); i++)
    {
        if (sock_d == i->second.first)
            return i->second.second;
    }
    // logging("socket descriptor not found in the sockets map: ", WARNING, NULL, 0);
    return vector<Server>();
}

vector<int> Socket_map::get_sockets(void)
{
    vector<int> res;
    for (map_sock::iterator i = sock_map.begin(); i != sock_map.end(); i++)
        res.push_back(i->second.first);
    return res;
}

int Socket_map::size()
{
    return sock_map.size();
}

void Socket_map::print_map(void)
{
    cout << YELLOW;
    for (map_sock::iterator i = sock_map.begin(); i != sock_map.end(); i++)
    {
        cout << "Host:Port: " << i->first << ", ";
        cout << "Socket: " << i->second.first << ", ";
        cout << "Server.size: " << i->second.second.size() << endl;
    }
    cout << RESET;
}

// string Socket_map::get_port(int sock_d) {
//     for (map_sock::iterator i = sock_map.begin(); i != sock_map.end(); i++)
//     {
//         if (sock_d == i->second.first)
//         {
//             cout << (i->first) << endl;
//             return "";
//             // return (i->first).substr(".");
//         }
//     }
//     return "";
// }