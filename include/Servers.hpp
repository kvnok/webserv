#pragma once

#include "ServerBlock.hpp"

class Servers
{
	private:
		vector<ServerBlock> _servers;
		vector<pollfd> _fds;
	public:
		Servers();
		Servers(vector<ServerBlock> &serverBlocks);
		void start();
		void setFds();
		~Servers();
		void handleNewConnection(int& i);
		void handleExistingConnection(int& i);
};

void handleRequestAndMakeResponse(vector<char>buffer, const int clientSocket);