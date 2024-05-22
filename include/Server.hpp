#ifndef SERVER_H
# define SERVER_H

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

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define RESET   "\033[0m"

class Server
{
	private:
		int port;
		int serverFd;
		int maxClients;
		std::vector<std::string> server_name;
		std::vector<pollfd> pollFds;
		struct sockaddr_in address;
		int opt;
		int addrlen;
		int newSocket;
		std::string path;
		std::string request;
		std::string response;
	public:
		Server( void );
		Server( int port);
		~Server( void );
		void setPort(int port);
		int getPort();
		std::vector<std::string> getServerName();
		bool setPortNumberFromFile(std::string line);
		bool setServerNameFromFile(std::string line);
		void setSocket();
		void handleConfigFile(std::string configFile);
		int  connect( void );
		void getClient(int client_fd);
		void serveHtml(Server& server);
		void parseRequestPath(std::string request);
};

std::string firstWord(std::string line);
bool containsOnlyNumbers(const std::string& str);
int skipFirstWord(std::string line);
int howManyServerName(std::string line);
std::string skipWhiteSpace(std::string line);


#endif