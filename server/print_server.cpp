#include <header.hpp>

void printf_locations(vector<Location> &locations) {
    vector<Location>::iterator it = locations.begin();
    set<string>::iterator it2;
    int                 m = 1;

    while (it != locations.end())
    {
        cout << GREEN << "----------------------" << "LOCATION " << m++ << "------------------------------" << RESET << endl;
        cout << "Route : " << it->getRoute() << endl;
        cout << "Methods : ";
        for (it2 = it->getMethods().begin(); it2 != it->getMethods().end(); it2++)
            cout << *it2 << " ";
        cout << endl;
        cout << "Default Pages : ";
        for (vector<string>::iterator it2 = it->getDefaultPages().begin(); it2 != it->getDefaultPages().end(); it2++)
            cout << *it2 << " ";
        cout << endl;
        cout << "Root : " << it->getRoot() << endl;
        cout << "Directory Listings : " << it->getDirListings() << endl;
        cout << "Upload Path : " << it->getUploadPath() << endl;
        cout << "CGI Extensions : ";
        for (set<string>::iterator it2 = it->getCgiExt().begin(); it2 != it->getCgiExt().end(); it2++)
            cout << *it2 << " ";
        cout << endl;
        cout << "CGI Timeout : " << it->getCgiTimeout() << endl;
        it++;
    }
}

void print_servers(vector<Server> &servers){
    vector<Server>::iterator it = servers.begin();
    vector<Location>::iterator it2;
    int                         m = 1;

    while (it != servers.end())
    {
        cout << RED << "-------------------" << "SERVER " << m++ << "---------------------------------" << RESET << endl;
        it2 = it->getLocations().begin();
        cout << "Hostname : " << it->getHostname() << endl;
        cout << "Ports : ";
        set<int>::iterator it3 = it->getPorts().begin();
        for (; it3 != it->getPorts().end(); it3++)
            cout << *it3 << " ";
        cout << endl;
        cout << "Body Size : " << it->getBodySize() << endl;
        cout << "Server Names : ";
        for (set<string>::iterator it3 = it->getServerNames().begin(); it3 != it->getServerNames().end(); it3++)
            cout << *it3 << " ";
        cout << endl;
        cout << "Error Pages : ";
        for (map<int, string>::iterator it3 = it->getErrorPages().begin(); it3 != it->getErrorPages().end(); it3++)
            cout <<  it3->first <<  " : " << it3->second << " ";
        cout << endl;
        printf_locations(it->getLocations());
        it++;
    }
}