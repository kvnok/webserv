#pragma once

#include "Parser.hpp"

typedef Location Loc;

class smartLocs {
	private:
		map<string, Loc> _locs;
	public:
		smartLocs();
		smartLocs(pServerBlock &block);
		smartLocs(const smartLocs& other); //CHANGED
		~smartLocs();

		smartLocs& operator=(const smartLocs& other);

		map<string, Loc> &get_locs();
		Loc &get_loc(string &path);
		void add_loc(Loc &loc);
		void set_locs(map<string, Loc> &locs);
		void set_loc(Loc &loc);
};
