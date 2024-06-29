#pragma once
#include "stuff.hpp"
#include "ParseServerBlock.hpp"

class Config {
	private:
		vector<pServerBlock> _server_blocks;
	public:
		Config();
		Config(vector<pServerBlock> server_blocks);
		~Config();
		vector<pServerBlock> get_server_blocks() const;
		void set_server_blocks(vector<pServerBlock> server_blocks);
		void add_server_block(pServerBlock server_block);
		void print_server_blocks();
};
