#pragma once

#include "ServerBlock.hpp"
#include "Connection.hpp"

class Servers
{
	private:
		vector<ServerBlock> _serverBlocks;
		vector<Connection> _connections;
		vector<pollfd> _fds;
	public:
		Servers();
		Servers(vector<ServerBlock> &serverBlocks);
		~Servers();
		void	start();
		void	setFds();
		void	handleNewConnection(int i);
		void	handleExistingConnection(int& i);
		void	readRequest(Connection& connection);
		void	parseRequest(Connection& connection, ServerBlock& serverBlock);
		void	executeRequest(Connection& connection);
		void	writeResponse(Connection& connection);
		void	closeConnection(int &i);
};

void handleRequestAndMakeResponse(vector<char>buffer, const int clientSocket);