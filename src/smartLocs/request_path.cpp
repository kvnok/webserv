#include "Connection.hpp"
#include "ServerBlock.hpp"
#include "autoindex.hpp"

void print_the_loc(Loc loc) {
	cout << "loc: " << endl;
	cout << "path: " << loc.get_path() << endl;
	cout << "root: " << loc.get_root() << endl;
	cout << "index: " << loc.get_index() << endl;
	cout << "autoindex: " << loc.get_autoindex() << endl;
	cout << "is_cgi: " << loc.get_is_cgi() << endl;
	cout << "cgi_extension: " << loc.get_cgi_extension() << endl;
	cout << "is_redirect: " << loc.get_is_redirect() << endl;
	cout << "redirect: " << loc.get_redirect() << endl;
	cout << "redirect_code: " << loc.get_redirect_code() << endl;
}

static void parse_path(string &path, string &folder, string &file) {
	if (path == "/") {
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
	string root_and_file = root + "/" + file;
	root_and_file = regex_replace(root_and_file, regex("//+"), "/");

	if (file.empty()) { // no file, check for index
		string root_and_index = root + "/" + server.getIndex();
		root_and_index = regex_replace(root_and_index, regex("//+"), "/");
		ifstream stream(root_and_index);
		if (stream.is_open() && is_directory(root_and_index) == false) {
			path = root_and_index;
			stream.close();
		}
		else { // can't open index
			request.setStatusCode(404);
		}
		//TODO: is it correct that we dont close the stream if 'is_dir' is true?
	}
	else if (!file.empty()) { // check for file
		ifstream stream(root_and_file);
		// check if file is a folder

		if (stream.is_open() && is_directory(root_and_file) == false) {
			path = root_and_file;
			stream.close();
		}
		else { // can't open file part of the path
			request.setStatusCode(404);
		}
		//TODO: is it correct that we dont close the stream if 'is_dir' is true?
	}

	// int statusCode = request.getStatusCode();
	// if (statusCode != 200 && (statusCode < 300 || statusCode >= 400)) {
	// 	path = err_pages[statusCode];
	// }
	// Moved this to response, since in every case we need to get the path of the status code this way.
}

void check_locs(Connection& connection, Request &request, string &folder, string &file, string &path, map<int, string> err_pages, smartLocs sLocs) {
	Loc loc;
	try {
		loc = sLocs.get_loc(folder);
	}
	catch (invalid_argument &e) {
		request.setStatusCode(404);
		// path = folder + "/" + err_pages[404];
		// setting the path of a error page will happen in response;
		return;
	}
	// print_the_loc(loc);

	string root = loc.get_root();
	string root_and_file = root + "/" + file;
	root_and_file = regex_replace(root_and_file, regex("//+"), "/");

	if (file.empty()) { // no file, check for index
		if (loc.get_index() != "")
		{
			string root_and_index = root + "/" + loc.get_index();
			root_and_index = regex_replace(root_and_index, regex("//+"), "/");
			ifstream stream(root_and_index);
			if (stream.is_open() && is_directory(root_and_index) == false) {
				path = root_and_index;
				stream.close();
			}
			else { // can't open index;
				request.setStatusCode(404);
			}
		}
		else if (loc.get_autoindex() == true) {
			request.setIsAutoindex(true);
			path = root;
		}
		else { // no index, no autoindex
			request.setStatusCode(404);
		}
	}
	else if (!file.empty()) { // check for file
		if (loc.get_is_cgi() == true)
		{
			//cout << "identified that its an CGI" << endl;
			// dont set it to true, check if it is indeed a valid cgi path, then set it to true.
			request.setIsCGI(true);
			request.setCGIExtension(loc.get_cgi_extension());
		}
		ifstream stream(root_and_file);
		if (stream.is_open() && is_directory(root_and_file) == false) {
			path = root_and_file;
			stream.close();
		}
		else { // can't open file part of the path
			request.setStatusCode(404);
		}
		//TODO: is it correct that we dont close the stream if 'is_dir' is true?
	}

	// int statusCode = request.getStatusCode();
	// // if (statusCode != 200 && (statusCode < 300 || statusCode >= 400)) {
	// // 	path = err_pages[statusCode];
	// // }
	//this will be done in the response;
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
	Loc loc;
	loc = sLocs.get_loc(folder);
	string redirect = loc.get_redirect();
	request.setStatusCode(loc.get_redirect_code());
	folder = redirect;
}

void print_smartLocs(smartLocs sLocs) {
	map<string, Loc> locs = sLocs.get_locs();
	for (auto it = locs.begin(); it != locs.end(); it++)
		it->second.print_location();
}

void request_path_handler(Connection& connection) {
	Request& request = connection.getRequest();
	ServerBlock serverBlock = connection.getServer();
	string path = request.getPath();
	if (request.getStatusCode() != 200)
		cout << YEL << "WE SHOULDNT CHANGE THE STATUS CODE!!!!" << RESET << endl;
	request.setStatusCode(200); // why set the status code to 200 without checks?
	string folder;
	string file;
	parse_path(path, folder, file);

	map<int, string> err_pages = serverBlock.getErrorPages();
	smartLocs sLocs;
	sLocs.set_locs(serverBlock.getSmartLocs().get_locs());
	// print_smartLocs(sLocs);

	if (is_this_a_redirect(folder, file, sLocs)) {
		do_the_redirect(request, folder, sLocs);
		file = "";
	}
	if (folder == "/" || folder == "")
		check_baseline(request, file, path, serverBlock, err_pages);
	else
		check_locs(connection, request, folder, file, path, err_pages, sLocs);
	request.setPath(path);
}
