#include <header.hpp>

void    displayTimestamp(const char *color)
{
    time_t  now;
    char    time_buffer[30];

    now = time(NULL);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S: ", localtime(&now));
    cout << color << time_buffer;
}

void    logging(const string& msg, int level, Server *srv, int port)
{
    if (level == INFO)
    {
        displayTimestamp(GREEN);
        if (srv)
            cout << "Server: " << srv->get_index();
        if (port)
            cout << " (port: " << port << ") ";
        cout << "[INFO]: " << msg << RESET << endl; 
    }
    else if (level == WARNING)
    {
        displayTimestamp(YELLOW);
        if (srv)
            cout << "Server: " << srv->get_index();
        if (port)
            cout << " (port: " << port << ") ";
        cout << "[WARNING]: " << msg << RESET << endl; 
    }
    else if (level == ERROR)
    {
        displayTimestamp(RED);
        if (srv)
            cout << "Server: " << srv->get_index();
        if (port)
            cout << " (port: " << port << ") ";
        cout << "[ERROR]: " << msg << RESET << endl; 
    }
}