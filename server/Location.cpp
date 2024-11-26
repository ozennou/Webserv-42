#include <Location.hpp>

Location::Location() {
    dirlst = false;
    exact = false;
    redirect.first = -1;
    cgi_timeout = 60;  //https://www.baeldung.com/linux/nginx-timeouts#:~:text=NGINX%20fastcgi_*%20Timeouts&text=fastcgi_connect_timeout%20%E2%80%93%20maximum%20time%20to%20connect,FastCGI%20server%20response%20(default%2060)
}

Location::~Location() {
}

std::string Location::getRoute() const {
    return route;
}

void Location::setRoute(const std::string& newRoute) {
    route = newRoute;
}

std::set<std::string> &Location::getMethods() {
    return methods;
}

void Location::addMethod(const std::string& method) {
    if (method != "GET" && method != "POST" && method != "DELETE")
        throw logic_error("Error: invalid method :" + method);
    methods.insert(method);
}

void Location::removeMethod(const std::string& method) {
    methods.erase(method);
}

int Location::isMethodAllowed(const std::string& method) const {
    if (methods.empty())
        return 1;
    return methods.find(method) != methods.end();
}

std::vector<std::string> &Location::getDefaultPages() {
    return default_pages;
}

void Location::addDefaultPage(const std::string& defaultPage) {
    if (defaultPage.empty())
        throw logic_error("Error: empty index page");
    default_pages.push_back(defaultPage);
}

void Location::removeDefaultPage(const std::string& defaultPage) {
    default_pages.erase(std::remove(default_pages.begin(), default_pages.end(), defaultPage), default_pages.end());
}

std::string Location::getRoot() const {
    return root;
}

void Location::setRoot(const std::string& newRoot) {
    if (newRoot.empty())
        throw logic_error("Error: empty index page");
    root = newRoot;
}

bool Location::getDirListings() const {
    return dirlst;
}

void Location::setDirListings(bool newDirListings) {
    dirlst = newDirListings;
}

bool Location::getExact() const {
    return exact;
}

void Location::setExact(bool newExact) {
    exact = newExact;
}

std::set<std::string> &Location::getCgiExt() {
    return cgi_ext;
}

void Location::addCgiExt(const std::string& cgiExt) {
    if (cgiExt != "java" && cgiExt != "sh" && cgiExt != "php" && cgiExt != "py")
        throw logic_error("Error: Invalid CGI extension :" + cgiExt);
    cgi_ext.insert(cgiExt);
}

void Location::removeCgiExt(const std::string& cgiExt) {
    cgi_ext.erase(cgiExt);
}

std::string Location::getUploadPath() const {
    return upload_path;
}

void Location::setUploadPath(const std::string& newUploadPath) {
    if (newUploadPath.empty())
        throw logic_error("Error: invalid updload path");
    upload_path = newUploadPath;
}

unsigned int Location::getCgiTimeout() const {
    return cgi_timeout;
}

void Location::setCgiTimeout(unsigned int newCgiTimeout) {
    cgi_timeout = newCgiTimeout;
}

void Location::ready_location(void)
{
    if (route.empty() || root.empty())
        throw logic_error(string("Error: the location :") + (route.empty() ? "ROUTE" : "ROOT") + " directive is mandatory");
    if (upload_path.empty())
        throw logic_error("Error: the location : UPLOAD directive is manadatory");
    if (!methods.size())
    {
        this->addMethod("POST");
        this->addMethod("GET");
        this->addMethod("DELETE");
    }
}

pair<int, string>   Location::getRedirect() const{
    return redirect;
}

void                Location::setRedirect(int code, string path){
    redirect = pair<int, string>(code, path);
}
