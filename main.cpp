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
    try {
        servers = check_config_file(ac, av);
        // print_servers(servers);
        init_servers(servers);
    } catch(exception &e) {
        cerr << RED << e.what() << RESET << endl;
        return 1;
    }
    return 0;
}