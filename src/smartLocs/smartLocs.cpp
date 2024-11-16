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

smartLocs::smartLocs(const smartLocs& other) { *this = other; } //CHANGED

smartLocs::~smartLocs() {}

smartLocs &smartLocs::operator=(const smartLocs &other) {
	if (this == &other)
		return *this;
	_locs = other._locs;
	return *this;
}

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
