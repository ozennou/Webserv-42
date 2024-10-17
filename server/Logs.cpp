#include <header.hpp>

void    displayTimestamp(const char *color)
{
    time_t  now;
    char    time_buffer[30];

    now = time(NULL);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S ", localtime(&now));
    cerr << color << time_buffer;
}

void    logging(const string& msg, int level, Server *srv, int port)
{
    if (level == INFO)
    {
        displayTimestamp(GREEN);
        if (srv)
            cerr << "Server " << srv->get_index();
        if (port)
            cerr << "(port: " << port << ")";
        cerr << " [INFO] :" << msg << RESET << endl; 
    }
    else if (level == WARNING)
    {
        displayTimestamp(YELLOW);
        if (srv)
            cerr << "Server " << srv->get_index();
        if (port)
            cerr << "(port: " << port << ")";
        cerr << "[WARNING] :" << msg << RESET << endl; 
    }
    else if (level == ERROR)
    {
        displayTimestamp(RED);
        if (srv)
            cerr << "Server " << srv->get_index();
        if (port)
            cerr << "(port: " << port << ")";
        cerr << "[ERROR] :" << msg << RESET << endl; 
    }
}