/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   Servers.hpp                                        :+:    :+:            */
/*                                                     +:+                    */
/*   By: jvorstma <jvorstma@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2024/07/16 14:18:12 by jvorstma      #+#    #+#                 */
/*   Updated: 2024/07/16 18:14:46 by jvorstma      ########   odam.nl         */
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
		void	parseRequest(Connection& connection);
		void	executeRequest(Connection& connection);
		void	writeResponse(Connection& connection);
		void	closeConnection(int &i);
};
