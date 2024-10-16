#include <header.hpp>

int hasExtension(string file_name) {
    size_t pos = file_name.find_last_of('.');
    if (pos == string::npos || pos == 0)
        return false;
    std::string extension = file_name.substr(pos + 1);
    if (extension == "conf")
        return true;
    return false;
}

void print_tokens(vector<pair<int, string> > &tk)
{
    for (vector<pair<int, string> >::iterator i = tk.begin(); i != tk.end(); i++)
    {
        cout << get_special((*i).first) <<  "-->" << (*i).second << endl;
    }
}

vector<Server>    check_config_file(int ac, char **av) {
    string                      config_file;
    vector<pair<int, string> >   tokens;

    if (ac == 1)
        config_file = DEFAULT_CONF;
    else if (ac == 2)
        config_file = av[1];
    else
        throw invalid_argument("Usage : ./webserv <config_file>(optional)");
    if (!hasExtension(config_file))
        throw invalid_argument("Error: The file must have a .conf extension");
    ifstream conf_file(config_file.c_str());
    if (!conf_file.is_open())
        throw invalid_argument("Error : Opening the config file");
    tokens = tokenizer(conf_file);
    conf_file.close();
    return parsing(tokens);
}