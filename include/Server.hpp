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
		vector<int>			sockets;
		map<string, string>	mime_types;

	public:
		Server();
		~Server();

		void	set_index(const int &_index);
		const int &get_index(void);

		string getHostname() const;
		void setHostname(const string& _hostname);

		set<int> &getPorts();
		void addPort(int port);
		void removePort(int port);

		size_t getBodySize() const;
		void setBodySize(size_t _bodySize);

		set<string> &getServerNames();
		void addServerName(const string& serverName);
		void removeServerName(const string& serverName);

		map<int, string> &getErrorPages();
		string getErrorPage(int errorCode) const;
		void setErrorPage(int errorCode, const string& errorPage);
		void removeErrorPage(int errorCode);

		map<string, string> &getMimeTypes();
		map<string, string>::iterator getMimeType(string &ext);
		map<string, string>::iterator MimeEnd();
		void	setMimeType(string &ext, string &type);
		void	printMimeType(); //ril
		
		vector<Location> &getLocations();
		void addLocation(const Location& location);

		void	ready_server(void);
		bool	isValidIPv4(const string& ip);
} ;