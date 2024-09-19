/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Servers.cpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/16 14:17:40 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/08/06 11:16:47 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "Servers.hpp"

Servers::Servers( void ) { }

Servers::Servers( vector<ServerBlock> &serverBlocks) {
	this->_serverBlocks = serverBlocks;
	this->setFds();
}

Servers::~Servers( void ) { }

vector<ServerBlock> &Servers::get_serverBlocks() { return this->_serverBlocks; }
vector<Connection>  &Servers::get_connections() { return this->_connections; }
vector<pollfd>      &Servers::get_fds() { return this->_fds; }

void    Servers::set_serverBlocks(vector<ServerBlock> &serverBlocks) { this->_serverBlocks = serverBlocks; }
void    Servers::set_connections(vector<Connection> &connections) { this->_connections = connections; }
void    Servers::set_fds(vector<pollfd> &fds) { this->_fds = fds; }

void    Servers::setFds() {
	for (int i = 0; i < this->_serverBlocks.size(); i++) {
		pollfd fd;
		fd.fd = this->_serverBlocks[i].getFd();
		fd.events = POLLIN;
		this->_fds.push_back(fd);
	}
}
