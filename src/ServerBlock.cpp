#include "ServerBlock.hpp"

ServerBlock::ServerBlock( void ) {
    this->_port = 0;
    this->_listen = "";
	this->_host = "";
    if (!this->_server_names.empty())
        this->_server_names[0] = "";
    this->_client_max_body_size = "";
    this->_index = "";
    this->_error_pages[404] = "";
    this->_root = "";
    this->_opt = 1;
    this->_serverFd = 0;
    this->max_clients = MAX_CLIENTS;
} // do we need a default constructor? or should we alwasy get the constructor whti pServerBlock?

vector<string> ServerBlock::getServerName(){return this->_server_names;};
int ServerBlock::getPort(){return this->_port;};
string ServerBlock::getHost(){return this->_host;};
string ServerBlock::getListen(){return this->_listen;};
string ServerBlock::getMaxBody(){return this->_client_max_body_size;};
string ServerBlock::getIndex(){return this->_index;};
map<int, string> ServerBlock::getErrorPages(){return this->_error_pages;};
string ServerBlock::getRoot(){return this->_root;};
int ServerBlock::getFd(){return this->_serverFd;};
int ServerBlock::getMaxClients(){return this->max_clients;};
string ServerBlock::getCurrentPath(){return this->_currentPath;};
vector<Location> ServerBlock::getLocations(){return this->_locations;};
smartLocs ServerBlock::getSmartLocs(){return this->_smartLocs;};
void ServerBlock::setSmartLocs(pServerBlock &block){this->_smartLocs = smartLocs(block);};

ServerBlock::ServerBlock(pServerBlock& block) {
    this->_listen = block.get_listen();
    this->_host = block.get_host();
    this->_port = block.get_port();
    this->_server_names = block.get_server_names();
    this->_client_max_body_size = block.get_client_max_body_size();
	this->_index = block.get_index();
    this->_error_pages = block.get_error_pages();
	this->_root = block.get_root();
	this->_locations = block.get_locations();
    this->_opt = 1;
    this->_serverFd = 0;
    this->max_clients = MAX_CLIENTS;
    this->_currentPath = block.get_root();
    this->setSocket();
    smartLocs temp_smartLocs(block);
    this->_smartLocs.set_locs(temp_smartLocs.get_locs());
}

void ServerBlock::setBind() {
    if (this->_serverFd == 0)
        throw runtime_error("socket not set");
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(this->_port);
    int addrlen = sizeof(address);
	if (bind(this->_serverFd, (struct sockaddr *)&address, addrlen) == -1) {
        perror("bind failed"); //why do we use perror here?
        throw runtime_error("bind failed");
    }
}

void ServerBlock::setListen() {
    if (this->_serverFd == 0)
        throw runtime_error("socket not set");
    if (listen(this->_serverFd, this->getMaxClients()) == -1)
        throw runtime_error("listen failed");
}

void ServerBlock::setSocket() {
	if ((this->_serverFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        throw runtime_error("socket failed");
	if (setsockopt(this->_serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &this->_opt, sizeof(this->_opt)) == -1)
        throw runtime_error("setsockopt failed");
    if (fcntl(this->_serverFd, F_SETFL, fcntl(this->_serverFd, F_GETFL, 0) | O_NONBLOCK) == -1) //set it to non-blocking mode
        throw runtime_error("fcntl failed");
    this->setBind();
    this->setListen();
}

ServerBlock::~ServerBlock( void ) {}

ServerBlock& ServerBlock::operator=(const ServerBlock& other) {
    this->_port = other._port;
    this->_listen = other._listen;
    this->_host = other._host;
    this->_server_names = other._server_names;
    this->_client_max_body_size = other._client_max_body_size;
    this->_index = other._index;
    this->_error_pages = other._error_pages;
    this->_root = other._root;
    this->_locations = other._locations;
    this->_serverFd = other._serverFd;
    this->_opt = other._opt;
    this->max_clients = other.max_clients;
    this->_currentPath = other._currentPath;
    this->_smartLocs = other._smartLocs;
    return *this;
}
