#include "Connection.hpp"

static void parse_path(string &path, string &folder, string &file) {
	size_t pos = path.find_last_of('/');
	if (pos == string::npos) {
		folder = "/";
		file = path;
	}
	else {
		folder = path.substr(0, pos);
		file = path.substr(pos + 1);
	}
}

void request_path_handler(string &path, Request &request) {
	// first check if its just / or /file or /folder/
	string folder;
	string file;
	parse_path(path, folder, file);
	cout << YEL << "folder: " << folder << RESET << endl;
	cout << YEL << "file: " << file << RESET << endl;

	
	
	smartLocs sLocs;
	sLocs.set_locs(request.getServer().getSmartLocs().get_locs());
}
