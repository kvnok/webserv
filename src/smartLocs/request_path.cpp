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

void check_baseline(Request &request, string &file, string &path, Server *server, map<int, string> err_pages) {
	string root = server->getRoot();
	string root_and_file = root + "/" + file;
	request.setStatusCode(200);

	if (file.empty()) { // no file, check for index
		string root_and_index = root + "/" + server->getIndex();
		ifstream stream(root_and_index);
		if (stream.is_open()) {
			path = root_and_index;
			stream.close();
		}
		else { // can't open index
			request.setStatusCode(404);
		}
	}
	else if (!file.empty()) { // check for file
		ifstream stream(root_and_file);
		if (stream.is_open()) {
			path = root_and_file;
			stream.close();
		}
		else { // can't open file part of the path
			request.setStatusCode(404);
		}
	}

	int statusCode = request.getStatusCode();
	if (statusCode != 200) {
		path = root + "/" + err_pages[statusCode];
	}
}

void check_locs(Request &request, string &folder, string &file, string &path, map<int, string> err_pages, smartLocs sLocs) {
	Loc loc;
	try {
		loc = sLocs.get_loc(folder);
	}
	catch (invalid_argument &e) {
		request.setStatusCode(404);
		path = folder + "/" + err_pages[404];
		return;
	}

	string root = loc.get_root();
	string root_and_file = root + "/" + file;
	request.setStatusCode(200);

	if (file.empty()) { // no file, check for index
		if (loc.get_index() != "")
		{
			string root_and_index = root + "/" + loc.get_index();
			ifstream stream(root_and_index);
			if (stream.is_open()) {
				path = root_and_index;
				stream.close();
			}
			else { // can't open index	
				request.setStatusCode(404);
			}
		}
		else if (loc.get_autoindex() == "on") {
			// do autoindex
			// for now just 404 because autoindex is not implemented yet
			request.setStatusCode(404);
		}
		else { // no index, no autoindex
			request.setStatusCode(404);
		}
	}
	else if (!file.empty()) { // check for file
		ifstream stream(root_and_file);
		if (stream.is_open()) {
			path = root_and_file;
			stream.close();
		}
		else { // can't open file part of the path
			request.setStatusCode(404);
		}
	}

	int statusCode = request.getStatusCode();
	if (statusCode != 200) {
		path = root + "/" + err_pages[statusCode];
	}
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
		check_baseline(request, file, path, server, err_pages);
	}
	else {
		check_locs(request, folder, file, path, err_pages, sLocs);
	}
}
