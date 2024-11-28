
#include "Servers.hpp"

ServerBlock*	Servers::getFdsServerBlock(int const fd) {
    for (size_t j = 0; j < this->_serverBlocks.size(); j++) {
        if (this->_serverBlocks[j].getFd() == fd)
            return (&this->_serverBlocks[j]);
    }
    return (nullptr);
}

Connection* 	Servers::getFdsClient(int const fd) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getFd() == fd)
            return (&this->_connections[j]);
    }
    return (nullptr);
}

Connection*		Servers::getOtherFdsClient(int const fd) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getOtherFD() == fd)
            return (&this->_connections[j]);
    }
    return (nullptr);
}

bool    Servers::isServerFd(int const fd) {
    for (size_t j = 0; j < this->_serverBlocks.size(); j++) {
        if (this->_serverBlocks[j].getFd() == fd)
            return (true);
    }
    return (false);
}

bool    Servers::isClientFd(int const fd) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getFd() == fd)
            return (true);
    }
    return (false);
}

bool    Servers::isOtherFd(int const fd) {
    for (size_t j = 0; j < this->_connections.size(); j++) {
        if (this->_connections[j].getOtherFD() == fd)
            return (true);
    }
    return (false);
}