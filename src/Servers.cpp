
#include "Servers.hpp"

Servers::Servers( void ) { }

Servers::Servers( vector<ServerBlock> &serverBlocks) {
	this->_serverBlocks = serverBlocks;
	this->setFds();
}

Servers::~Servers( void ) { 
	this->_connections.clear();
	for (size_t i = 0; i < this->_serverBlocks.size(); i++) {
		if (this->_serverBlocks[i].getFd() != -1)
			close (this->_serverBlocks[i].getFd());
	}
	this->_fds.clear(); 
	//all fd's should be coverd by the serverblocks and connection closers
	//so no fds should be remaining open
}

vector<ServerBlock> &Servers::get_serverBlocks() { return this->_serverBlocks; }
vector<Connection>  &Servers::get_connections() { return this->_connections; }
vector<pollfd>      &Servers::get_fds() { return this->_fds; }

void    Servers::set_serverBlocks(vector<ServerBlock> &serverBlocks) { this->_serverBlocks = serverBlocks; }
void    Servers::set_connections(vector<Connection> &connections) { this->_connections = connections; }
void    Servers::set_fds(vector<pollfd> &fds) { this->_fds = fds; }

void    Servers::setFds() {
	for (size_t i = 0; i < this->_serverBlocks.size(); i++) {
		pollfd fd;
		fd.fd = this->_serverBlocks[i].getFd();
		fd.events = POLLIN;
		this->_fds.push_back(fd);
	}
}
