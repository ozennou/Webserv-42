#include <Server.hpp>

Server::Server() {
    body_size = 1000000; // https://stackoverflow.com/questions/28476643/default-nginx-client-max-body-size
    hostname = DEFAULT_HOST;
}

Server::~Server() {
}

void	Server::set_index(const int &_index)
{
    index = _index;
}

const int &Server::get_index(void)
{
    return index;
}

std::string Server::getHostname() const {
    return hostname;
}

bool Server::isValidIPv4(const string& ip) {
    stringstream ss(ip);
    string segment;
    vector<int> segments;

    while (getline(ss, segment, '.')) {
        if (segment.empty() || segment.size() > 3) return false;
        int num = 0;
        stringstream ss2(segment);
        if (!(ss2 >> num)) return false;
        if (ss2.fail() || !ss2.eof()) return false;
        if (num < 0 || num > 255) return false;
        segments.push_back(num);
    }
    return segments.size() == 4;
}

void Server::setHostname(const std::string& _hostname) {
    if (_hostname.empty() || !isValidIPv4(_hostname))
        throw logic_error("Error: invalid host name");
    hostname = _hostname;
}

std::set<int> &Server::getPorts() {
    return ports;
}

void Server::addPort(int port) {
    ports.insert(port);
}

void Server::removePort(int port) {
    ports.erase(port);
}

size_t Server::getBodySize() const {
    return body_size;
}

void Server::setBodySize(size_t _bodySize) {
    body_size = _bodySize;
}

std::set<std::string> &Server::getServerNames() {
    return server_names;
}

void Server::addServerName(const std::string& serverName) {
    server_names.insert(serverName);
}

void Server::removeServerName(const std::string& serverName) {
    server_names.erase(serverName);
}

string Server::getErrorPage(int errorCode) const {
    if (error_pages.find(errorCode) == error_pages.end())
        return "";                                              // Replace with default error page
    return error_pages.at(errorCode);
}

std::map<int, std::string> &Server::getErrorPages() {
    return error_pages;
}

void Server::setErrorPage(int errorCode, const std::string& errorPage) {
    error_pages[errorCode] = errorPage;
}

void Server::removeErrorPage(int errorCode) {
    error_pages.erase(errorCode);
}

std::vector<Location> &Server::getLocations() {
    return locations;
}

void Server::addLocation(const Location& location) {
    locations.push_back(location);
}

void Server::ready_server(void)
{
    if (!ports.size())
        throw logic_error("Error: ports are mandatory to the server");
    if (!server_names.size())
        this->addServerName("");
}