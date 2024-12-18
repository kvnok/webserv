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

#include "smartLocs.hpp"

using namespace std;

#define MAX_CLIENTS 10
#define GREEN   "\033[32m"

class smartLocs;

class ServerBlock
{
	private:
		int _port;
		string _listen;
		string _host;
		vector<string> _server_names;
		unsigned int	_client_max_body_size;
		string _index;
		map<int, string> _error_pages;
		string _root;
		vector<Location> _locations;
		int _serverFd;
		int _opt;
		unsigned int num_of_clients;
		int max_clients;
		string _currentPath;
		smartLocs _smartLocs;
	public:
		ServerBlock( void );
		ServerBlock(pServerBlock& blocks);
		ServerBlock(const ServerBlock& other);
		~ServerBlock( void );

		ServerBlock& operator=(const ServerBlock& other);
		// -------- Getters -------- //
		string getHost();
		string getListen();
		int getPort();
		vector<string> getServerName();
		unsigned int	getMaxBody();
		string getIndex();
		map<int, string> getErrorPages();
		string getRoot();
		int getFd();
		int getMaxClients();
		string getCurrentPath();
		vector<Location> getLocations();
		unsigned int getNumOfClients();
		// ------------------------- //
		void setSocket();
		void setBind();
		void setListen();
		void incrementNumOfClients();
		void decrementNumOfClients();
		// ------------------------- //
		smartLocs getSmartLocs();
		void setSmartLocs(pServerBlock &block);
} ;
