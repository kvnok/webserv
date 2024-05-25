#include "../include/stuff.hpp"
#include "../include/parser.hpp"

Parser::Parser(std::string config_file) : _config_file(config_file) {}

Parser::~Parser() {}

/*
loop to start of the key
loop to end of the key
loop to start of the value
loop to end of the value
repeat last two steps until end of line



*/
void parse_line(std::string &line) {
	
	
	
}

void Parser::parse() {
	std::ifstream file(_config_file.c_str());
	if (!file.is_open()) {
		std::cerr << "could not open file" << std::endl;
		return;
	}
	std::string line;
	while (std::getline(file, line)) {
		// std::cout << line << std::endl;
		parse_line(line);
		std::cout << line << std::endl;
	}
}

/*
removing whitespace
end of the line is semi colon or { or }
some things are mandatory
some things are optional
some things are repeated
some things are nested
some things are not
some things are in a specific order
some things are not
*/
/*
spaces in front
splitting on spaces
array of things that can be repeated on same line
and array of things that can be repeated on multiple lines


so an array of locations
each location is:
- root
- index
- autoindex
*/

