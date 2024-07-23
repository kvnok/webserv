#include "Connection.hpp"
#include "ServerBlock.hpp"

static void parse_path(string &path, string &folder, string &file) {
	if (path == "/") {
		cout << YEL << "path is /" << RESET << endl;
		folder = "/";
		file = "";
		return;
	}
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

void check_baseline(Request &request, string &file, string &path, ServerBlock server, map<int, string> err_pages) {
	string root = server.getRoot();
	cout << YEL << "root: " << root << RESET << endl;
	string root_and_file = root + "/" + file;
	request.setStatusCode(200);

	if (file.empty()) { // no file, check for index
		string root_and_index = root + "/" + server.getIndex();
		cout << YEL << "index: " << server.getIndex() << RESET << endl; // "index.html
		cout << YEL << "root_and_index: " << root_and_index << RESET << endl;
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
		path = err_pages[statusCode];
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
		else if (loc.get_autoindex() == true) {
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
		path = err_pages[statusCode];
	}
}

void ok_print_server_block(ServerBlock &serverBlock) {
	cout << "server block: " << endl;
	cout << "port: " << serverBlock.getPort() << endl;
	cout << "root: " << serverBlock.getRoot() << endl;
	cout << "index: " << serverBlock.getIndex() << endl;
	// cout << "error pages: " << endl;
	// map<int, string> err_pages = serverBlock.getErrorPages();
	// for (map<int, string>::iterator it = err_pages.begin(); it != err_pages.end(); it++) {
	// 	cout << it->first << ": " << it->second << endl;
	// }
	// cout << "smart locations: " << endl;
	// smartLocs sLocs = serverBlock.getSmartLocs();
	// map<string, Loc> locs = sLocs.get_locs();
	// for (map<string, Loc>::iterator it = locs.begin(); it != locs.end(); it++) {
	// 	cout << "loc: " << it->first << endl;
	// 	cout << "root: " << it->second.get_root() << endl;
	// 	cout << "index: " << it->second.get_index() << endl;
	// 	cout << "autoindex: " << it->second.get_autoindex() << endl;
	// }
}

void request_path_handler(string &path, Request &request, ServerBlock serverBlock) {
	cout << RED << "start path: " << path << RESET << endl;
	// first check if its just / or /file or /folder/
	string folder;
	string file;
	parse_path(path, folder, file);
	cout << YEL << "folder: " << folder << RESET << endl;
	cout << YEL << "file: " << file << RESET << endl;

	ServerBlock server = serverBlock;

	map<int, string> err_pages = server.getErrorPages();
	smartLocs sLocs;
	sLocs.set_locs(server.getSmartLocs().get_locs());

	if (folder == "/") {
		cout << YEL << "baseline" << RESET << endl;
		check_baseline(request, file, path, server, err_pages);
	}
	else {
		cout << YEL << "locs" << RESET << endl;
		check_locs(request, folder, file, path, err_pages, sLocs);
	}
	cout << RED << "end path: " << path << RESET << endl;
	request.setPath(path);
}
