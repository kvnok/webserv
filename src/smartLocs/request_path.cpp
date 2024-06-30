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
	string root = server->getRoot();
	string root_and_file = root + "/" + file;

	if (file.empty()) { // no file, check for index
		string index = server->getIndex();
		string root_and_index = root + "/" + index;
		ifstream stream(root_and_index);
		if (stream.is_open()) {
			path = root_and_index;
			stream.close();
		}
		else { // 404
			string err_page = err_pages[404];
			if (err_page.empty()) {
				throw runtime_error("404 error page not found");
			}
			path = root + "/" + err_page;
		}
	}
	else { // 404
		string err_page = err_pages[404];
		if (err_page.empty()) {
			throw runtime_error("404 error page not found");
		}
		path = root + "/" + err_page;
	}
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

// leave open the check for opening the file

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
/*
ifstream file(str);
	if (!file.is_open()) {
		throw runtime_error("block: invalid root: " + s[1]);
	}
	file.close();
*/
/*
	if (file.empty()) { // no file, check for index
		string index = server->getIndex();
		string root_and_index = root + "/" + index;
		ifstream file(root_and_index);
		if (file.is_open()) {
			path = root_and_index;
			file.close();
		}
		else {
			// check for autoindex
			string autoindex = server->getSmartLocs().get_autoindex();
			if (autoindex == "on") {
				// autoindex
				// check for 404
				string err_page = err_pages[404];
				if (err_page.empty()) {
					throw runtime_error("404 error page not found");
				}
				path = root + "/" + err_page;
			}
			else {
				// 404
				string err_page = err_pages[404];
				if (err_page.empty()) {
					throw runtime_error("404 error page not found");
				}
				path = root + "/" + err_page;
			}
		}
	}
}
*/