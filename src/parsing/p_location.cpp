#include "parser.hpp"

void parse_location(RAWSERV &s, Location &location, int &i)
{
	if (s[i].size() != 3 || s[i][0] != "location" || s[i][1][0] != '/' || s[i][2] != "{")
		throw logic_error("location block must start with location {");
	location.set_path(s[i][1].substr(0, s[i][1].find(";")));
	i++;

	for (; i < s.size(); i++) {
		if (s[i][0] == "}" && s[i].size() == 1)
			break;
		if (s[i].size() < 2)
			throw logic_error("directive must have a value");
		s[i][1] = s[i][1].substr(0, s[i][1].find(";"));
		if (s[i][0] == "root") {
			location.set_root(s[i][1]);
		}
		else if (s[i][0] == "index") {
			location.set_index(s[i][1]);
		}
		else if (s[i][0] == "autoindex") {
			location.set_autoindex(s[i][1]);
		}
		else if (s[i][0] == "cgi_extension") {
			location.set_cgi_extension(s[i][1]);
			location.set_is_cgi(true);
		}
		else if (s[i][0] == "return") {
			location.set_redirect(s[i][1]);
			location.set_is_redirect(true);
		}
		else {
			throw logic_error("unknown directive : " + s[i][0]);
		}
	}
}
