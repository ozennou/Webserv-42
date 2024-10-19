#include <header.hpp>
#include <Server.hpp>

void    lol()
{
    system("leaks webserv");
    system("lsof -c webserv");
}

int main(int ac, char **av) {
    // atexit(lol);
    vector<Server>  servers;
    Socket_map      sock_map;
    try {
        servers = check_config_file(ac, av);
        init_servers(servers, sock_map);
        // print_servers(servers);
        // sock_map.print_map();
        server_loop(servers, sock_map);
    } catch(exception &e) {
        cerr << RED << e.what() << RESET << endl;
        return 1;
    }
    return 0;
}