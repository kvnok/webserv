
#pragma once

#include "ServerBlock.hpp"
#include "Connection.hpp"
#include <fcntl.h>

using namespace std;

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
		void	prepExec(Connection& connection);
		void	getStatusCodePage(Connection& connection);
		void	addFdToPoll(Connection& connection, size_t& i);
		void	executeMethod(Connection& connection, size_t& i);
		void	delFdFromPoll(Connection& connection, size_t& i);
		void	closeConnection(Connection& connection, size_t& i);
		vector<ServerBlock> &get_serverBlocks();
		vector<Connection>	&get_connections();
		vector<pollfd>		&get_fds();
		void	set_serverBlocks(vector<ServerBlock> &serverBlocks);
		void	set_connections(vector<Connection> &connections);
		void	set_fds(vector<pollfd> &fds);
};
