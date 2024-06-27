#include "Parser.hpp"

set<string> l_directives = {"root", "index", "autoindex", "cgi_extension", "return", "deny"};

bool is_valid_location_directive(const string& directive) {
	return l_directives.count(directive) > 0;
}

map<string, function<void(vector<string> &, Location&)>> l_directive_funcs = {
	{"root", l_root},
	{"index", l_index},
	{"autoindex", l_autoindex},
	{"cgi_extension", l_cgi_extension},
	{"return", l_redirect},
	{"deny", l_deny}
};

void parse_location(RAWSERV &s, Location &location, int &i)
{
	if (s[i].size() != 3 || s[i][0] != "location" || s[i][1][0] != '/' || s[i][2] != "{")
		throw logic_error("location block must start with location {");
	s[i][1] = s[i][1].substr(0, s[i][1].find(";"));
	// check if the path is already used by other locations
	for (auto& loc : s) {
		if (loc[0] == "location" && loc[1] == s[i][1] && &loc != &s[i]) {
			throw logic_error("location path already used");
		}
	}
	if (s[i][1] == "/")
		throw logic_error("location path cannot be just /");
	if (s[i][1][0] != '/')
		throw logic_error("location path must start with /");
	// if path is something else then /, letter, digit, or _ then throw error
	for (size_t j = 1; j < s[i][1].size(); j++) {
		if (!isalnum(s[i][1][j]) && s[i][1][j] != '_' && s[i][1][j] != '/')
			throw logic_error("location path can only contain letters, digits, and _");
	}
	location.set_path(s[i][1]);
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
