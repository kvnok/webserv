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
		Connection();
		Connection(vector<Server> &server);
		void start();
		void setFds();
		~Connection();
		void handleNewConnection(int& i);
		void handleExistingConnection(int& i);
};

void handleRequestAndMakeResponse(vector<char>buffer, const int clientSocket);
void close_uploaded_image_page(int clientSocket);