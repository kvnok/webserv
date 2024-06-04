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

using namespace std;

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define GREEN   "\033[32m"

class Server
{
	private:
		int _port;
		vector<string> _server_names;
		int _client_max_body_size;
		string _index;
		map<int, string> _error_pages;
		string _root;
		vector<Location> _locations;
		int serverFd;
		struct sockaddr_in address;
		int opt;
		int addrlen;
		int newSocket;
	public:
		Server( void );
		Server(ServerBlock& blocks);
		~Server( void );
		void setPort(int port);
		// -------- Getters -------- //
		int getPort();
		vector<string> getServerName();
		int getMaxBody();
		string getIndex();
		map<int, string> getErrorPages();
		string getRoot();
		int getFd();
		// ------------------------- //
		void setSocket();
};

