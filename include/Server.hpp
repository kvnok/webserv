#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <poll.h>
#include "config.hpp"

using namespace std;

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
// #define RED     "\033[31m"
// #define GREEN   "\033[32m"
// #define RESET   "\033[0m"

class Server
{
	private:
		int _port;
		string _server_name;
		int _client_max_body_size;
		string _index;
		string _error_page;
		string _root;
		vector<Location> _locations;

		int serverFd;
		vector<pollfd> pollFds;
		struct sockaddr_in address;
		int opt;
		int addrlen;
		int newSocket;
		string path;
		string request;
		string response;
	public:
		Server( void );
		Server(ServerBlock& blocks);
		~Server( void );
		void setPort(int port);
		// -------- Getters -------- //
		int getPort();
		string getServerName();
		int getMaxBody();
		string getIndex();
		string getErrorPage();
		string getRoot();
		int getFd();
		// ------------------------- //
		void setSocket();
		int  connect( void );
		void serveHtml(Server& server);
		void parseRequestPath(string request);
};
