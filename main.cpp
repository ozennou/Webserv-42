#include "header.hpp"
#include <Server.hpp>

void    lol()
{
    system("leaks webserv");
    system("lsof -c webserv");
}

void signal_handler(int signal)
{
    if (signal == SIGINT || signal == SIGQUIT)
    {
        cout << "\n";
        cout << "=============================================\n";
        cout << "      🚨 Server Interrupted 🚨\n";
        cout << "=============================================\n";
        cout << "=============================================\n";
        cout << "      🌐 Thank you for using our WebServer 🌐\n";
        cout << "      🚀 Developed by: MOZENNOU and MLOUAZIR 🚀\n";
        cout << "=============================================\n";
        cout << endl;
        set_value(1);
    }
}

int main(int ac, char **av) {
    // atexit(lol);
    vector<Server>  servers;
    Socket_map      sock_map;
    try {
        signal(SIGPIPE, signal_handler);  //https://stackoverflow.com/questions/4584904/what-causes-the-broken-pipe-error
        signal(SIGINT, signal_handler);
        signal(SIGQUIT, signal_handler);
        servers = check_config_file(check_flags(ac, av));
        init_servers(servers, sock_map);
        // print_servers(servers);
        // sock_map.print_map();
        poll_loop(servers, sock_map);
    } catch(exception &e) {
        cerr << RED << e.what() << RESET << endl;
        return 1;
    }
    return 0;
}