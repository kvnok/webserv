#include "smartLocs.hpp"

void test_smartLocs(Config &config) {
	vector<pServerBlock> server_blocks = config.get_server_blocks();
	for (int i = 0; i < server_blocks.size(); i++)
	{
		color_print(GRN, "pServerBlock: ", i);
		pServerBlock server_block = server_blocks[i];
		smartLocs smartLocs(server_block);
		smartLocs.print_locs();
	}
}
