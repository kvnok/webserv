#include "parser.hpp"

std::set<std::string> l_directives = {"root", "index", "autoindex", "cgi_extension", "return"};

bool is_valid_location_directive(const std::string& directive) {
	return l_directives.count(directive) > 0;
}

std::map<std::string, std::function<void(vector<string> &, Location&)>> l_directive_funcs = {
	{"root", l_root},
	{"index", l_index},
	{"autoindex", l_autoindex},
	{"cgi_extension", l_cgi_extension},
	{"return", l_redirect}
};

void parse_location(RAWSERV &s, Location &location, int &i)
{
	if (s[i].size() != 3 || s[i][0] != "location" || s[i][1][0] != '/' || s[i][2] != "{")
		throw logic_error("location block must start with location {");
	location.set_path(s[i][1].substr(0, s[i][1].find(";")));
	i++;

	for (; i < s.size(); i++) {
		// basic error checking
		if (s[i][0] == "}" && s[i].size() == 1)
			break;
		if (s[i].size() < 2)
			throw logic_error("directive must have a value");
		
		// is this an allowed directive
		if (!is_valid_location_directive(s[i][0]))
			throw logic_error("unknown directive : " + s[i][0]);
		// call the handler
		l_directive_funcs[s[i][0]](s[i], location);
	}
}
/*
check if the mandatory directives are present in the location block
*/
/*
s[i][1] = s[i][1].substr(0, s[i][1].find(";"));
*/
