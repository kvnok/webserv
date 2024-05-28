#include "config.hpp"

Config::Config() {}

Config::Config(vector<ServerBlock> server_blocks)
{
	_server_blocks = server_blocks;
}

Config::~Config() {}

vector<ServerBlock> Config::get_server_blocks() const { return _server_blocks; }

void Config::set_server_blocks(vector<ServerBlock> server_blocks) { _server_blocks = server_blocks; }

void Config::add_server_block(ServerBlock server_block) { _server_blocks.push_back(server_block); }
