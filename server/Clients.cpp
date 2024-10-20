#include <header.hpp>

Clients::Clients()
{
}

Clients::~Clients()
{
}

map<int, int>   Clients::get_map(void)
{
    return client_map;
}

void    Clients::add_client(int cliend_d, int sock_d)
{
    client_map.insert(make_pair(cliend_d, sock_d));
}

int     Clients::exist_client(int client_d)
{
    if (client_map.find(client_d) == client_map.end())
        return 0;
    return 1;
}

int     Clients::get_sock_d(int client_d)
{
    if (exist_client(client_d))
        return (client_map.find(client_d)->second);
    return -1;
}

void    Clients::remove_client(int client_d)
{
    client_map.erase(client_d);
}

void    Clients::clients_loop(Socket_map &sock_map)
{
    (void)sock_map;
    cout << RED << client_map.size() << RESET << endl;
    for (map<int, int>::iterator i = client_map.begin(); i != client_map.end();)
    {
        char bf[1024];
        int lenght = recv(i->first, bf, 1024, 0);
        if (lenght < 0)
        {
            i++;
            continue;
        }
        else if (!lenght)
        {
            logging("Client desconnected", INFO, NULL, 0);
            close(i->first);
            i = client_map.erase(i);
        }
        else
        {
            //you should contunue reading here
            i++;
        }
    }
}