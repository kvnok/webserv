#include "Connection.hpp"
#include "ServerBlock.hpp"

static void parse_path(string &path, string &folder, string &file) {
	if (path == "/") {
		// cout << YEL << "path is /" << RESET << endl;
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
	// cout << YEL << "root: " << root << RESET << endl;
	string root_and_file = root + "/" + file;
	request.setStatusCode(200);

	if (file.empty()) { // no file, check for index
		// cout << YEL << "no file, check for index" << RESET << endl;
		string root_and_index = root + "/" + server.getIndex();
		// cout << YEL << "index: " << server.getIndex() << RESET << endl; // "index.html
		// cout << YEL << "root_and_index: " << root_and_index << RESET << endl;
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
		// cout << YEL << "check for file" << RESET << endl;
		ifstream stream(root_and_file);
		if (stream.is_open()) {
			// cout << YEL << "file found" << RESET << endl;
			path = root_and_file;
			stream.close();
		}
		else { // can't open file part of the path
			// cout << YEL << "file not found" << RESET << endl;
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
		// cout << RED << "loc not found" << RESET << endl;
		request.setStatusCode(404);
		path = folder + "/" + err_pages[404];
		return;
	}

	string root = loc.get_root();
	string root_and_file = root + "/" + file;
	request.setStatusCode(200);

	if (file.empty()) { // no file, check for index
		// cout << YEL << "no file, check for index" << RESET << endl;
		if (loc.get_index() != "")
		{
			// cout << YEL << "index: " << loc.get_index() << RESET << endl;
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
			// cout << YEL << "autoindex" << RESET << endl;
			// request.setStatusCode(404);
			
		}
		else { // no index, no autoindex
			// cout << YEL << "no index, no autoindex" << RESET << endl;
			request.setStatusCode(404);
		}
	}
	else if (!file.empty()) { // check for file
		// cout << YEL << "check for file" << RESET << endl;
		ifstream stream(root_and_file);
		if (stream.is_open()) {
			// cout << YEL << "file found" << RESET << endl;
			// cout << GRN << "loc root: " << root << RESET << endl;
			// cout << GRN << "file: " << file << RESET << endl;
			path = root_and_file;
			stream.close();
		}
		else { // can't open file part of the path
			// cout << YEL << "file not found" << RESET << endl;
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
}

void request_path_handler(Connection& connection) {
	Request& request = connection.getRequest();
	ServerBlock serverBlock = connection.getServer();
	string path = request.getPath();
	// cout << BOLD << "REQUEST PATH HANDLER" << RESET << endl;
	// cout << BOLD << request.getPath() << RESET << endl;
	// ok_print_server_block(serverBlock);
	// first check if its just / or /file or /folder/
	string folder;
	string file;
	parse_path(path, folder, file);
	// cout << YEL << "folder: |" << folder << "|" << RESET << endl;
	// cout << YEL << "file: " << file << RESET << endl;

	map<int, string> err_pages = serverBlock.getErrorPages();
	smartLocs sLocs;
	sLocs.set_locs(serverBlock.getSmartLocs().get_locs());
	sLocs.set_locs(serverBlock.getSmartLocs().get_locs());

	if (folder == "/" || folder == "") {
		// cout << BOLD << "CHECKING IN BASELINE" << RESET << endl;
		check_baseline(request, file, path, serverBlock, err_pages);
	}
	else {
		// cout << BOLD << "CHECKING IN LOC BLOCKS" << RESET << endl;
		check_locs(request, folder, file, path, err_pages, sLocs);
	}
	// replace // with /
	path = regex_replace(path, regex("//+"), "/");
	request.setPath(path);
	cout << BOLD << "END OF REQUEST PATH HANDLER: " << request.getPath() << RESET << endl;
}
