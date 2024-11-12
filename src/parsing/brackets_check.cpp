#include "Parser.hpp"

void check_brackets(RAWSERV &raw_server) {
	stack<string> s;
	for (int i = 0; i < (int)raw_server.size(); i++) {//CHANGED cast to int
		for (int j = 0; j < (int)raw_server[i].size(); j++) {//CHANGED cast to int
			if (raw_server[i][j] == "{") {
				s.push("{");
			} else if (raw_server[i][j] == "}") {
				if (s.empty()) {
					throw logic_error("brackets are not closed correctly");
				}
				s.pop();
			}
		}
	}
	if (!s.empty()) {
		throw logic_error("brackets are not closed correctly");
	}
}
