#include "Connection.hpp"
#include "ServerBlock.hpp"
#include "autoindex.hpp"

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
	root_and_file = regex_replace(root_and_file, regex("//+"), "/");
	request.setStatusCode(200);

	if (file.empty()) { // no file, check for index
		cout << YEL << "no file, check for index" << RESET << endl;
		string root_and_index = root + "/" + server.getIndex();
		root_and_index = regex_replace(root_and_index, regex("//+"), "/");
		cout << YEL << "index: " << server.getIndex() << RESET << endl; // "index.html
		cout << YEL << "root_and_index: " << root_and_index << RESET << endl;
		ifstream stream(root_and_index);
		if (stream.is_open() && is_directory(root_and_index) == false) {
			path = root_and_index;
			stream.close();
		}
		else { // can't open index
			request.setStatusCode(404);
		}
	}
	else if (!file.empty()) { // check for file
		cout << YEL << "check for file" << RESET << endl;
		ifstream stream(root_and_file);
		// check if file is a folder

		if (stream.is_open() && is_directory(root_and_file) == false) {
			cout << YEL << "file found" << RESET << endl;
			path = root_and_file;
			stream.close();
		}
		else { // can't open file part of the path
			cout << YEL << "file not found" << RESET << endl;
			request.setStatusCode(404);
		}
	}

	int statusCode = request.getStatusCode();
	if (statusCode != 200) {
		path = err_pages[statusCode];
	}
}

void check_locs(Connection& connection, Request &request, string &folder, string &file, string &path, map<int, string> err_pages, smartLocs sLocs) {
	Loc loc;
	try {
		loc = sLocs.get_loc(folder);
	}
	catch (invalid_argument &e) {
		cout << RED << "loc not found" << RESET << endl;
		request.setStatusCode(404);
		path = folder + "/" + err_pages[404];
		return;
	}

	string root = loc.get_root();
	string root_and_file = root + "/" + file;
	root_and_file = regex_replace(root_and_file, regex("//+"), "/");
	request.setStatusCode(200);

	if (file.empty()) { // no file, check for index
		cout << YEL << "no file, check for index" << RESET << endl;
		if (loc.get_index() != "")
		{
			if (loc.get_cgi_extension() != "")
			{
				request.setIsCGI(true);
				request.setCGIextension(loc.get_cgi_extension());
			}
			cout << YEL << "index: " << loc.get_index() << RESET << endl;
			string root_and_index = root + "/" + loc.get_index();
			root_and_index = regex_replace(root_and_index, regex("//+"), "/");
			ifstream stream(root_and_index);
			if (stream.is_open() && is_directory(root_and_index) == false) {
				path = root_and_index;
				stream.close();
			}
			else { // can't open index
				cout << YEL << "cant open index" << RESET << endl;
				request.setStatusCode(404);
			}
		}
		else if (loc.get_autoindex() == true) {
			cout << YEL << "autoindex" << RESET << endl;
			// do autoindex
			request.setIsAutoindex(true);
			// for now just 404 because autoindex is not implemented yet
			request.setStatusCode(404);
		}
		else { // no index, no autoindex
			cout << YEL << "no index, no autoindex" << RESET << endl;
			request.setStatusCode(404);
		}
	}
	else if (!file.empty()) { // check for file
		cout << YEL << "check for file" << RESET << endl;
		ifstream stream(root_and_file);
		if (stream.is_open() && is_directory(root_and_file) == false) {
			cout << YEL << "file found" << RESET << endl;
			cout << GRN << "loc root: " << root << RESET << endl;
			cout << GRN << "file: " << file << RESET << endl;
			path = root_and_file;
			stream.close();
		}
		else { // can't open file part of the path
			cout << YEL << "file not found" << RESET << endl;
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

bool is_this_a_redirect(string &folder, string &file, smartLocs sLocs) {
	if (folder != "" && file != "") {
		return false;
	}
	if ((folder == "" || folder == "/") && file == "") {
		return false;
	}
	if (file != "") {
		return false;
	}
	Loc loc;
	try {
		loc = sLocs.get_loc(folder);
	}
	catch (invalid_argument &e) {
		return false;
	}
	if (loc.get_is_redirect() == false) {
		return false;
	}
	return true;
}

void do_the_redirect(Request &request, string &folder, smartLocs sLocs) {
	cout << BOLD << "DO THE REDIRECT" << RESET << endl;
	Loc loc;
	loc = sLocs.get_loc(folder);
	string redirect = loc.get_redirect();
	cout << "redirect: |" << redirect << "|" << endl;
	request.setStatusCode(loc.get_redirect_code());
	folder = redirect;
}

void print_smartLocs(smartLocs sLocs) {
	cout << GRN << "smartLocs: " << RESET << endl;
	map<string, Loc> locs = sLocs.get_locs();
	for (auto it = locs.begin(); it != locs.end(); it++) {
		cout << "loc: |" << it->first << "|" << endl;
		it->second.print_location();
	}
}

void request_path_handler(Connection& connection) {
	Request& request = connection.getRequest();
	ServerBlock serverBlock = connection.getServer();
	string path = request.getPath();
	connection.getRequest().setIsAutoindex(false);
	// cout << BOLD << "REQUEST PATH HANDLER" << RESET << endl;
	// cout << BOLD << request.getPath() << RESET << endl;
	// ok_print_server_block(serverBlock);
	// first check if its just / or /file or /folder/
	string folder;
	string file;
	parse_path(path, folder, file);
	cout << YEL << "folder: |" << folder << "|" << RESET << endl;
	cout << YEL << "file: " << file << RESET << endl;

	map<int, string> err_pages = serverBlock.getErrorPages();
	smartLocs sLocs;
	sLocs.set_locs(serverBlock.getSmartLocs().get_locs());
	// print_smartLocs(sLocs);

	if (is_this_a_redirect(folder, file, sLocs)) {
		do_the_redirect(request, folder, sLocs);
		file = "";
	}
	if (folder == "/" || folder == "") {
		cout << BOLD << "CHECKING IN BASELINE" << RESET << endl;
		check_baseline(request, file, path, serverBlock, err_pages);
	}
	else {
		// cout << BOLD << "CHECKING IN LOC BLOCKS" << RESET << endl;
		check_locs(connection, request, folder, file, path, err_pages, sLocs);
	}
	// replace // with /
	request.setPath(path);
	// cout << BOLD << "END OF REQUEST PATH HANDLER: " << request.getPath() << RESET << endl;
}
