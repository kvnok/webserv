#include "smartLocs.hpp"

smartLocs::smartLocs() {}

smartLocs::smartLocs(ServerBlock &block) {
	vector<Loc> locs = block.get_locations();
	for (int i = 0; i < locs.size(); i++)
	{
		Loc loc = locs[i];
		_locs[loc.get_path()] = loc;
	}
}

smartLocs::~smartLocs() {}

void smartLocs::print_locs() {
	map<string, Loc>::iterator it = _locs.begin();
	for (; it != _locs.end(); it++)
	{
		color_print(BLU, "Location: ", it->first);
		it->second.print_location();
	}
}

map<string, Loc> &smartLocs::get_locs() { return _locs; }

Loc &smartLocs::get_loc(string &path) {
	map<string, Loc>::iterator it = _locs.begin();
	for (; it != _locs.end(); it++) {
		if (path.find(it->first) != string::npos) {
			return it->second;
		}
	}
	throw invalid_argument("No loc found");
}

void smartLocs::add_loc(Loc &loc) { _locs[loc.get_path()] = loc; }
void smartLocs::set_locs(map<string, Loc> &locs) { _locs = locs; }
void smartLocs::set_loc(Loc &loc) { _locs[loc.get_path()] = loc; }
