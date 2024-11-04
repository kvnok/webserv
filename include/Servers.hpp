
#pragma once

#include "ServerBlock.hpp"
#include "Connection.hpp"
#include <fcntl.h>

using namespace std;

#define BUFFER_SIZE 1024

class Servers
{
	private:
		vector<ServerBlock>	_serverBlocks;
		vector<Connection>	_connections;
		vector<pollfd>		_fds;
		Servers();
	public:
		Servers(vector<ServerBlock> &serverBlocks);
		~Servers();
		void	start();
		void	setFds();
		void	handleNewConnection(size_t i);
		void	handleExistingConnection(Connection& connection, size_t& i);
		void	readRequest(Connection& connection);
		void	executeRequest(Connection& connection);
		void	writeResponse(Connection& connection);
		void	closeConnection(Connection& connection, size_t& i);
		vector<ServerBlock> &get_serverBlocks();
		vector<Connection>	&get_connections();
		vector<pollfd>		&get_fds();
		void	set_serverBlocks(vector<ServerBlock> &serverBlocks);
		void	set_connections(vector<Connection> &connections);
		void	set_fds(vector<pollfd> &fds);
};
