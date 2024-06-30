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

void check_baseline(string &file, string &path, Server *server, map<int, string> err_pages) {

}

void check_locs(string &folder, string &file, string &path, map<int, string> err_pages, smartLocs sLocs) {

}

void request_path_handler(string &path, Request &request) {
	path = request.getPath();

	// first check if its just / or /file or /folder/
	string folder;
	string file;
	parse_path(path, folder, file);
	cout << YEL << "folder: " << folder << RESET << endl;
	cout << YEL << "file: " << file << RESET << endl;

	Server *server = request.getServer();

	map<int, string> err_pages = server->getErrorPages();
	smartLocs sLocs;
	sLocs.set_locs(server->getSmartLocs().get_locs());

	if (folder == "/") {
		check_baseline(file, path, server, err_pages);
	}
	else {
		check_locs(folder, file, path, err_pages, sLocs);
	}
}





// if file is "", then check for index or autoindex or 404



/*

	if (file == "") {
		if (folder == "/") {
			// check for index
			if (request.getServer()->getSmartLocs().get_index() != "") {
				path = folder + request.getServer()->getSmartLocs().get_index();
			}
			else {
				// check for autoindex
				if (request.getServer()->getSmartLocs().get_autoindex() == "on") {
					// autoindex
					// check for 404
					path = folder + "404.html";
				}
				else {
					// 404
					path = folder + "404.html";
				}
			}
		}
		else {
			// check for index
			if (request.getServer()->getSmartLocs().get_index() != "") {
				path = folder + "/" + request.getServer()->getSmartLocs().get_index();
			}
			else {
				// check for autoindex
				if (request.getServer()->getSmartLocs().get_autoindex() == "on") {
					// autoindex
					// check for 404
					path = folder + "/404.html";
				}
				else {
					// 404
					path = folder + "/404.html";
				}
			}
		}
	}
	else {
		// check for file
		path = folder + "/" + file;
	}

*/