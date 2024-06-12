#pragma once

#include "Server.hpp"

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
};



