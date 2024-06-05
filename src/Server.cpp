#include "Server.hpp"

Server::Server( void )
{
    this->_port = 0;
    this->_listen = "";
	this->_host = "";
    if (!this->_server_names.empty())
        this->_server_names[0] = "";
    this->_client_max_body_size = "";
    this->_index = "";
    this->_error_pages[404] = "";
    this->_root = "";
    this->serverFd = 0;
    this->max_clients = MAX_CLIENTS;
}


vector<string> Server::getServerName(){return this->_server_names;};
int Server::getPort(){return this->_port;};
string Server::getHost(){return this->_host;};
string Server::getListen(){return this->_listen;};
string Server::getMaxBody(){return this->_client_max_body_size;};
string Server::getIndex(){return this->_index;};
map<int, string> Server::getErrorPages(){return this->_error_pages;};
string Server::getRoot(){return this->_root;};
int Server::getFd(){return this->serverFd;};
int Server::getMaxClients(){return this->max_clients;};

Server::Server(ServerBlock& blocks)
{
    this->_listen = blocks.get_listen();
    this->_host = blocks.get_host();
    this->_port = blocks.get_port();
    this->_server_names = blocks.get_server_names();
    this->_client_max_body_size = blocks.get_client_max_body_size();
	this->_index = blocks.get_index();
    this->_error_pages = blocks.get_error_pages();
	this->_root = blocks.get_root();
	this->_locations = blocks.get_locations();
    this->setSocket();
}

void Server::setSocket()
{
	this->opt = 1;
	this->addrlen = sizeof(this->address);
	
	this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(this->_port);

	if ((this->serverFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		cerr << "socket failed";
	}
	if (setsockopt(this->serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &this->opt, sizeof(this->opt)) == -1)
	{
        cerr << "setsockopt failed";
    }
	if (bind(this->serverFd, (struct sockaddr *)&this->address, this->addrlen) == -1) {
        cerr << "bind failed";
    }

    if (listen(this->serverFd, this->getMaxClients()) == -1) {
        cerr << "listen failed";
    }
}

Server::~Server( void )
{

}
