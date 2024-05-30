#include "parser.hpp"

std::set<std::string> s_directives = {"server_name", "listen", "root", "error_page", "client_max_body_size", "index", "location"};

bool is_valid_server_block_directive(const std::string& directive) {
	return s_directives.count(directive) > 0;
}

std::map<std::string, std::function<void(vector<string> &, ServerBlock&)>> s_directive_funcs = {
	{"server_name", s_server_name},
	{"listen", s_listen},
	{"root", s_root},
	{"error_page", s_error_page},
	{"client_max_body_size", s_client_max_body_size},
	{"index", s_index}
};

void parse_server_block(RAWSERV &s, ServerBlock &block) {
	if (s.size() == 0)
		throw logic_error("empty server block");
	if (s[0].size() != 2 || s[0][0] != "server" || s[0][1] != "{")
		throw logic_error("first block is not a server block");

	for (int i = 1; s.size(); i++)
	{
		// basic error checking
		if (s[i][0] == "}" && s[i].size() == 1)
			break;
		if (s[i].size() < 2)
			throw logic_error("directive must have a value");

		// is this an allowed directive
		if (!is_valid_server_block_directive(s[i][0]))
			throw logic_error("unknown directive : " + s[i][0]);
		// if its location, then handle it separately
		if (s[i][0] == "location")
		{
			Location location;
			parse_location(s, location, i);
			block.add_location(location);
			continue;
		}
		// call the handler
		s_directive_funcs[s[i][0]](s[i], block);
	}
}
/*
check if the mandatory directives are present in the server block
*/
/*
s[i][1] = s[i][1].substr(0, s[i][1].find(";"));
*/
