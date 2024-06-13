#pragma once

#include "Server.hpp"
#include "httpRequest.hpp"
#include "httpResponse.hpp"

class Connection
{
	private:
		vector<Server> server;
		vector<pollfd> fds;
		vector<int> _firstConnection;
	public:
		Connection(/* args */);
		Connection( vector<Server> &server);
		void start();
		void setFds();
		~Connection();
		void handleNewConnection(int i);
		void handleExistingConnection(int i);
		void handleRequest(int clientSocket, Request& request, Response& response, int i);
};



