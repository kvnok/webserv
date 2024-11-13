#include "smartLocs.hpp"

void test_smartLocs(Config &config) {
	vector<pServerBlock> server_blocks = config.get_server_blocks();
	for (int i = 0; i < (int)server_blocks.size(); i++) //CHANGED cast to int
	{
		// cout << RED << "Testing smartLocs" << RESET << endl;
		color_print(GRN, "pServerBlock: ", i);
		pServerBlock server_block = server_blocks[i];
		smartLocs smartLocs(server_block);
		//smartLocs.print_locs();
	}
}
