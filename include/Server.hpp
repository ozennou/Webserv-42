#pragma once

#include <header.hpp>

#define DEFAULT_HOST "127.0.0.1" //I'm not sure about this default host

class Location;

class Server {
	private:
		int					index;
		string				hostname;
		set<int>			ports;
		size_t				body_size;
		set<string>			server_names;
		map<int, string>	error_pages;
		vector<Location>	locations;
	public:
		Server();
		~Server();

		void	set_index(const int &_index);
		const int &get_index(void);

		std::string getHostname() const;
		void setHostname(const std::string& _hostname);

		std::set<int> &getPorts();
		void addPort(int port);
		void removePort(int port);

		size_t getBodySize() const;
		void setBodySize(size_t _bodySize);

		std::set<std::string> &getServerNames();
		void addServerName(const std::string& serverName);
		void removeServerName(const std::string& serverName);

		std::map<int, std::string> &getErrorPages();
		std::string getErrorPage(int errorCode) const;
		void setErrorPage(int errorCode, const std::string& errorPage);
		void removeErrorPage(int errorCode);

		std::vector<Location> &getLocations();
		void addLocation(const Location& location);

		void	ready_server(void);
} ;