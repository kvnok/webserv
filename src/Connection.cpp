#include "Connection.hpp"

Connection::Connection( void )
{
	this->server = vector<Server>();
	this->fds = vector<pollfd>();
}

Connection::Connection( vector<Server> &server)
{
	this->server = server;
	this->setFds();
}

void Connection::setFds()
{
	vector<pollfd> fds;
	for (int i = 0; i < this->server.size(); i++)
	{
		pollfd fd;
		fd.fd = this->server[i].getFd();
		fd.events = POLLIN;
		this->fds.push_back(fd);
	}
}

void Connection::start()
{
}

Connection::~Connection( void )
{

}