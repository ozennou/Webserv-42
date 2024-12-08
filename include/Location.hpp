#pragma once

#include "header.hpp"

class Location {
    private:
        string              route;
        set<string>			methods;
        vector<string>      default_pages;
        string              root;
        bool                dirlst; //Turn on or off directory listing.
        set<string>         cgi_ext; //Execute CGI based on certain file extension
        string              upload_path;
        unsigned int        cgi_timeout;
        pair<int, string>   redirect;
        bool                exact; //exact location matching https://www.freecodecamp.org/news/the-nginx-handbook/
    public:
        Location();
        Location& operator=( const Location& obj );

        ~Location();

        std::string getRoute() const;
        void setRoute(const std::string& newRoute);

        std::set<std::string> &getMethods();
        void addMethod(const std::string& method);
        void removeMethod(const std::string& method);
        int  isMethodAllowed(const std::string& method) const;

        std::vector<std::string> &getDefaultPages();
        void addDefaultPage(const std::string& defaultPage);
        void removeDefaultPage(const std::string& defaultPage);

        std::string getRoot() const;
        void setRoot(const std::string& newRoot);

        bool getDirListings() const;
        void setDirListings(bool newDirListings);

        bool getExact() const;
        void setExact(bool newExact);

        std::set<std::string> &getCgiExt();
        void addCgiExt(const std::string& cgiExt);
        void removeCgiExt(const std::string& cgiExt);

        std::string getUploadPath() const;
        void setUploadPath(const std::string& newUploadPath);

        unsigned int getCgiTimeout() const;
        void setCgiTimeout(unsigned int newCgiTimeout);

        pair<int, string>   getRedirect() const;
        void                setRedirect(int code, string path);

        void ready_location(void);
} ;