#pragma once

#include "ServerBlock.hpp"

class Servers
{
	private:
		vector<ServerBlock> _servers;
		vector<Connection> _connections;
		vector<pollfd> _fds;
	public:
		Servers();
		Servers(vector<ServerBlock> &serverBlocks);
		~Servers();
		void	start();
		void	setFds();
		void	handleNewConnection(int i);
		void	handleExistingConnection(int i);
		void	readRequest(Connection& connection);
		void	parseRequest(Connection& connection);
		void	executeRequest(Connection& connection);
		void	writeResponse(Connection& connection);
		void	closeConnection(int &i);
};

void handleRequestAndMakeResponse(vector<char>buffer, const int clientSocket);