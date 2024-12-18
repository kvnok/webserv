
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
		void	handleExistingConnection(Connection& connection);
		void	prepExec(Connection& connection);
		void	executeMethod(Connection& connection);
		void	closeConnection(Connection& connection, size_t& i);
		void	deleteOtherFd(Connection& connection, size_t& i);
		vector<ServerBlock> &get_serverBlocks();
		vector<Connection>	&get_connections();
		vector<pollfd>		&get_fds();
		void	set_serverBlocks(vector<ServerBlock> &serverBlocks);
		void	set_connections(vector<Connection> &connections);
		void	set_fds(vector<pollfd> &fds);
		bool	isServerFd(int const fd);
		bool	isClientFd(int const fd);
		bool	isOtherFd(int const fd);
		ServerBlock*	getFdsServerBlock(int const fd);
		Connection*		getFdsClient(int const fd);
		Connection*		getOtherFdsClient(int const fd);
		
		void 			printFDS(); //for debug
		void 			startDebugPrint(string str, size_t i); //for debug
};
