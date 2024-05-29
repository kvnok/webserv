#include "parser.hpp"

void parse_server_block(RAWSERV &s, ServerBlock &block) {
	if (s.size() == 0)
		throw logic_error("empty server block");
	if (s[0].size() != 2 || s[0][0] != "server" || s[0][1] != "{")
		throw logic_error("first block is not a server block");

	for (int i = 1; s.size(); i++)
	{
		if (s[i][0] == "}" && s[i].size() == 1)
			break;
		if (s[i].size() < 2)
			throw logic_error("directive must have a value");
		if (s[i][0] == "server_name") {
			block.set_server_name(s[i][1]);
		}
		else if (s[i][0] == "listen") {
			block.set_listen(s[i][1]);
		}
		else if (s[i][0] == "root") {
			block.set_root(s[i][1]);
		}
		else if (s[i][0] == "error_page") {
			block.set_error_page(s[i][1]);
		}
		else if (s[i][0] == "client_max_body_size") {
			block.set_client_max_body_size(s[i][1]);
		}
		else if (s[i][0] == "index") {
			block.set_index(s[i][1]);
		}
		else if (s[i][0] == "location") {
			Location location;
			parse_location(s, location, i);
			block.add_location(location);
		}
		else {
			throw logic_error("unknown directive: " + s[i][0]);
		}
	}
}
