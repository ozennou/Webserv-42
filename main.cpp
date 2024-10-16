#include <header.hpp>
#include <Server.hpp>


int main(int ac, char **av) {
    vector<Server>  servers;
    try {
        servers = check_config_file(ac, av);
        print_servers(servers);
    } catch(exception &e) {
        cerr << RED << e.what() << RESET << endl;
    }
}