/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Servers.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/16 14:18:12 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/07/20 08:43:16 by jvorstma      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ServerBlock.hpp"
#include "Connection.hpp"

class Servers
{
	private:
		vector<ServerBlock> _serverBlocks;
		vector<Connection> _connections;
		vector<pollfd> _fds;
		Servers();
	public:
		Servers(vector<ServerBlock> &serverBlocks);
		~Servers();
		void	start();
		void	setFds();
		void	handleNewConnection(int i);
		void	handleExistingConnection(int& i);
		void	readRequest(Connection& connection);
		void	parseRequest(Connection& connection, ServerBlock& serverBlock);
		void	executeRequest(Connection& connection);
		void	writeResponse(Connection& connection);
		void	closeConnection(int &i);
		vector<ServerBlock> &get_serverBlocks();
		vector<Connection> &get_connections();
		vector<pollfd> &get_fds();
		void	set_serverBlocks(vector<ServerBlock> &serverBlocks);
		void	set_connections(vector<Connection> &connections);
		void	set_fds(vector<pollfd> &fds);
};

void handleRequestAndMakeResponse(vector<char>buffer, const int clientSocket, ServerBlock &serverBlock);