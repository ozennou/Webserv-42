#include <header.hpp>

string check_flags(int ac, char **av)
{
    string  res;
    size_t  i = 1;
    int     d1 = 0, d2 = 0;
    if (ac == 1)
    {
        logging_file("/dev/stderr", 1);
        return DEFAULT_CONF;
    }
    while (av[i])
    {
        string tmp(av[i]);
        if (tmp == "-c")
        {
            if (!av[i + 1] || d1)
                throw logic_error("Error: option \"-c\" requires file name");
            res = av[i + 1];
            i++;
            d1 = 1;
        }
        else if (tmp == "-l")
        {
            if (!av[i + 1] || d2)
                throw logic_error("Error: option \"-l\" requires file name");
            logging_file(av[i + 1], 1);
            i++;
            d2 = 1;
        }
        else if (tmp == "-h")
            throw logic_error("Usage: ./webserv [-c] [-l]\nOptions:\n  -h  : this help\n  -c  : set configuration file (default:./config_file/default.conf)\n  -l  : set logs file (default stderr)");
        else
            throw logic_error("Error: invalid option, use -h option for help");
        i++;
    }
    if (!d1)
        res = DEFAULT_CONF;
    if (!d2)
        logging_file("/dev/stderr", 1);
    else
    {
        ofstream tmp(logging_file(NULL, 0));
        if (!tmp.is_open())
            throw logic_error("Error: " + logging_file(NULL, 0) + ": No such file or directory");
        tmp.close();
    }
    return res;
}