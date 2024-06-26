#include "smartLocs.hpp"

void test_smartLocs(Config &config) {
	vector<ServerBlock> server_blocks = config.get_server_blocks();
	for (int i = 0; i < server_blocks.size(); i++)
	{
		color_print(GRN, "ServerBlock: ", i);
		ServerBlock server_block = server_blocks[i];
		smartLocs smartLocs(server_block);
		smartLocs.print_locs();
	}
}
