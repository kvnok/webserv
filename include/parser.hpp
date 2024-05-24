#pragma once

#include "stuff.hpp"

class Parser {
	private:
		std::string _config_file;
	public:
		Parser(std::string config_file);
		~Parser();
		void parse();
};
