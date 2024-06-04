#pragma once
#include "stuff.hpp"
#include "ServerBlock.hpp"

class Config {
	private:
		vector<ServerBlock> _server_blocks;
	public:
		Config();
		Config(vector<ServerBlock> server_blocks);
		~Config();
		vector<ServerBlock> get_server_blocks() const;
		void set_server_blocks(vector<ServerBlock> server_blocks);
		void add_server_block(ServerBlock server_block);
		void print_server_blocks();
};
