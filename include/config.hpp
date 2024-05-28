#pragma once
#include "stuff.hpp"
#include "serverBlock.hpp"

class Config {
	private:
		vector<ServerBlock> _server_blocks;
	public:
		Config();
		Config(vector<ServerBlock> server_blocks);
		~Config();
		vector<ServerBlock> get_server_blocks() const;
		void set_server_blocks(vector<ServerBlock> server_blocks);
};
