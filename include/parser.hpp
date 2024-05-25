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

template<typename T>
bool is_match(char c, const T& arg) {
    return c == arg;
}

template<typename T, typename... Args>
bool is_match(char c, const T& arg, const Args&... args) {
    if (c == arg) {
        return true;
    }
    return is_match(c, args...);
}
