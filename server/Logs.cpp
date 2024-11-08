#include <header.hpp>

string logging_file(const char *input, int opt)
{
    static string logs_path;
    if (opt)
        logs_path = input;
    return logs_path;
}

void    displayTimestamp(const char *color, ofstream &out)
{
    time_t  now;
    char    time_buffer[30];

    now = time(NULL);
    strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S: ", localtime(&now));
    out << color << time_buffer;
}

void    logging(const string& msg, int level, Server *srv, int port)
{
    if (!LOGS)
        return ;
    ofstream    out(logging_file(NULL, 0), ios::out | ios::app);
    bool        bl = (logging_file(NULL, 0) == "/dev/stderr")? 1 : 0;
    if (!out.is_open())
    {
        cerr << logging_file(NULL, 0) << ": no such file or directory" << endl; 
        return ;
    }
    if (level == INFO)
    {
        displayTimestamp(bl ? GREEN : "", out);
        if (srv)
            out << "Server: " << srv->get_index();
        if (port)
            out << " (" << srv->getHostname() << ":" << port << ") ";
        out << "[INFO]: " << msg << (bl ? RESET : "") << endl; 
    }
    else if (level == WARNING)
    {
        displayTimestamp(bl ? YELLOW : "", out);
        if (srv)
            out << "Server: " << srv->get_index();
        if (port)
            out << " (" << srv->getHostname() << ":" << port << ") ";
        out << "[WARNING]: " << msg << (bl ? RESET : "") << endl; 
    }
    else if (level == ERROR)
    {
        displayTimestamp(bl ? RED : "", out);
        if (srv)
            out << "Server: " << srv->get_index();
        if (port)
            out << " (" << srv->getHostname() << ":" << port << ") ";
        out << "[ERROR]: " << msg << (bl ? RESET : "") << endl; 
    }
    out.close();
}

int set_value(int i)
{
    static int m = 0;
    if (i == -1)
        return m;
    m = 1;
    return m;
}