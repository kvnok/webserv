#pragma once

#include <cstring>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <poll.h>
#include <unistd.h>
#include "Config.hpp"
#include "stuff.hpp"

using namespace std;

#define MAX_CLIENTS 10
#define GREEN   "\033[32m"

class ServerBlock
{
	private:
		int _port;
		string _listen;
		string _host;
		vector<string> _server_names;
		string _client_max_body_size;
		string _index;
		map<int, string> _error_pages;
		string _root;
		vector<Location> _locations;
		int _serverFd;
		int _opt;
		int newSocket;
		int max_clients;
		string _currentPath;
	public:
		ServerBlock( void );
		ServerBlock(pServerBlock& blocks);
		~ServerBlock( void );
		void setPort(int port);
		// -------- Getters -------- //
		string getHost();
		string getListen();
		int getPort();
		vector<string> getServerName();
		string getMaxBody();
		string getIndex();
		map<int, string> getErrorPages();
		string getRoot();
		int getFd();
		int getMaxClients();
		string getCurrentPath();
		vector<Location> getLocations();
		// ------------------------- //
		void setSocket();
		void setBind();
		void setListen();
} ;

