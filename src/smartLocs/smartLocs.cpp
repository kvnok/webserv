#include "smartLocs.hpp"

smartLocs::smartLocs() {}

smartLocs::smartLocs(pServerBlock &block) {
	vector<Loc> locs = block.get_locations();
	for (int i = 0; i < (int)locs.size(); i++) //CHANGED cast to int
	{
		Loc loc = locs[i];
		_locs[loc.get_path()] = loc;
	}
}

smartLocs::~smartLocs() {}

smartLocs &smartLocs::operator=(const smartLocs &other) {
	if (this == &other)
		return *this;
	_locs = other._locs;
	return *this;
}

// void smartLocs::print_locs() {
// 	map<string, Loc>::iterator it = _locs.begin();
// 	for (; it != _locs.end(); it++)
// 	{
// 		color_print(BLU, "Location: ", it->first);
// 		it->second.print_location();
// 	}
// }

map<string, Loc> &smartLocs::get_locs() { return _locs; }

Loc &smartLocs::get_loc(string &path) {
	map<string, Loc>::iterator it = _locs.find(path);
	if (it != _locs.end()) {
		return it->second;
	}
	throw invalid_argument("No loc found");
}

void smartLocs::add_loc(Loc &loc) { _locs[loc.get_path()] = loc; }
void smartLocs::set_locs(map<string, Loc> &locs) { _locs = locs; }
void smartLocs::set_loc(Loc &loc) { _locs[loc.get_path()] = loc; }
