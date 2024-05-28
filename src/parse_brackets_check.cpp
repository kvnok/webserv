#include "parser.hpp"

void check_brackets(vector<vector<string>> &raw_server) {
	stack<string> s;
	for (int i = 0; i < raw_server.size(); i++) {
		for (int j = 0; j < raw_server[i].size(); j++) {
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
