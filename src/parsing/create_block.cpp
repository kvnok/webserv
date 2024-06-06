#include "Parser.hpp"

set<string> s_directives = {"server_name", "listen", "root", "error_page", "client_max_body_size", "index", "location"};

bool is_valid_server_block_directive(const string& directive) {
	return s_directives.count(directive) > 0;
}

map<string, function<void(vector<string> &, ServerBlock&)>> s_directive_funcs = {
	{"server_name", s_server_name},
	{"listen", s_listen},
	{"root", s_root},
	{"error_page", s_error_page},
	{"client_max_body_size", s_client_max_body_size},
	{"index", s_index}
};

void check_location_block(Location loc) {
	if (loc.get_path().empty())
		throw logic_error("location missing path");
	
	if (loc.get_is_cgi() == false && loc.get_is_redirect() == false) {
		if (loc.get_root().empty()) {
			throw logic_error("location missing mandatory part");
		}
	}
	else if (loc.get_is_cgi() && loc.get_is_redirect()) {
		throw logic_error("location directive is not allowed for both cgi and redirect");
	}
	else if (loc.get_is_cgi()) {
		if (loc.get_root().empty()) {
			throw logic_error("location missing mandatory part");
		}
	}
	else if (loc.get_is_redirect()) {
		if (!loc.get_root().empty() || !loc.get_index().empty() || loc.get_is_autoindex_set() || loc.get_has_deny()) {
			throw logic_error("location illegal directive for redirect");
		}
	}
}

void check_manadatory_directives(ServerBlock &block) {
	if (block.get_listen().empty())
		throw logic_error("listen directive is mandatory");
	if (block.get_root().empty())
		throw logic_error("root directive is mandatory");
	if (block.get_index().empty())
		throw logic_error("index directive is mandatory");
	
	// and check the locations
	for (auto& location : block.get_locations()) {
		check_location_block(location);
		// for redirect, check if the return path is valid for going to another location
		if (location.get_is_redirect() == true) {
			bool found = false;
			for (auto& loc : block.get_locations()) {
				if (loc.get_path() == location.get_redirect()) {
					found = true;
					break;
				}
			}
			if (!found)
				throw logic_error("redirect path not found: " + location.get_redirect());
		}
	}
}

void parse_server_block(RAWSERV &s, ServerBlock &block) {
	if (s.size() == 0)
		throw logic_error("empty server block");
	if (s[0].size() != 2 || s[0][0] != "server" || s[0][1] != "{")
		throw logic_error("first block is not a server block");

	for (int i = 1; s.size(); i++) {
		// basic error checking
		if (s[i][0] == "}" && s[i].size() == 1)
			break;
		if (s[i].size() < 2)
			throw logic_error("directive must have a value");

		// is this an allowed directive
		if (!is_valid_server_block_directive(s[i][0]))
			throw logic_error("unknown directive : " + s[i][0]);
		// if its location, then handle it separately
		if (s[i][0] == "location") {
			Location location;
			parse_location(s, location, i);
			block.add_location(location);
			continue;
		}
		// call the handler
		s_directive_funcs[s[i][0]](s[i], block);
	}
	check_manadatory_directives(block);
}
