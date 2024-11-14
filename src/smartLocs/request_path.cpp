#include "Connection.hpp"
#include "ServerBlock.hpp"
#include "autoindex.hpp"

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

static void check_baseline(Request &request, string &file, string &path, ServerBlock server) { //CHANGED, del err_pages
	string root = server.getRoot();
	string root_and_file = root + "/" + file;
	root_and_file = regex_replace(root_and_file, regex("//+"), "/");

	//TODO here we need to implement DENY aswell
	if (file.empty()) { // no file, check for index
		string root_and_index = root + "/" + server.getIndex();
		root_and_index = regex_replace(root_and_index, regex("//+"), "/");
		ifstream stream(root_and_index);
		if (stream.is_open() && is_directory(root_and_index) == false) {
			path = root_and_index;
			stream.close();
		}
		else // can't open index
			request.setStatusCode(404); //CHECK
		//TODO: is it correct that we dont close the stream if 'is_dir' is true?
	}
	else if (!file.empty()) { // check for file
		ifstream stream(root_and_file);
		// check if file is a folder

		if (stream.is_open() && (is_directory(root_and_file) == false || (is_directory(root_and_file) && request.getMethod() == "POST"))) {
			path = root_and_file;
			stream.close();
		}
		else // can't open file part of the path
			request.setStatusCode(404); //CHECK
		//TODO: is it correct that we dont close the stream if 'is_dir' is true?
	}
}

static void check_locs(Request &request, string &folder, string &file, string &path, smartLocs sLocs) { //CHANGED del connection and err_pages
	Loc loc;
	try {
		loc = sLocs.get_loc(folder);
	}
	catch (invalid_argument &e) {
		request.setStatusCode(404); //CHECK
		return;
	}
	vector<string> deny = loc.get_deny();
	if (find(deny.begin(), deny.end(), request.getMethod()) != deny.end()) {
		request.setStatusCode(403);
		return ;
	}

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
			else // can't open index;
				request.setStatusCode(404); //CHECK
		}
		else if (loc.get_autoindex() == true) {
			request.setIsAutoindex(true);
			path = root;
		}
		else // no index, no autoindex
			request.setStatusCode(404); //CHECK
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
		else // can't open file part of the path
			request.setStatusCode(404); //CHECK
		//TODO: is it correct that we dont close the stream if 'is_dir' is true?
	}
}

static bool is_this_a_redirect(string &folder, string &file, smartLocs sLocs) {
	if (folder != "" && file != "")
		return false;
	if ((folder == "" || folder == "/") && file == "")
		return false;
	if (file != "")
		return false;
	Loc loc;
	try {
		loc = sLocs.get_loc(folder);
	}
	catch (invalid_argument &e) {
		return false;
	}
	if (loc.get_is_redirect() == false)
		return false;
	return true;
}

static void do_the_redirect(Request &request, string &folder, smartLocs sLocs) {
	Loc loc;
	loc = sLocs.get_loc(folder);
	string redirect = loc.get_redirect();
	request.setStatusCode(loc.get_redirect_code());
	folder = redirect;
}

void request_path_handler(Connection& connection) {
	Request& request = connection.getRequest();
	ServerBlock serverBlock = connection.getServer();
	string path = request.getPath();
	string folder;
	string file;
	parse_path(path, folder, file);

	smartLocs sLocs;
	sLocs.set_locs(serverBlock.getSmartLocs().get_locs());

	if (is_this_a_redirect(folder, file, sLocs)) {
		do_the_redirect(request, folder, sLocs);
		file = "";
	}
	if (folder == "/" || folder == "")
		check_baseline(request, file, path, serverBlock);
	else
		check_locs(request, folder, file, path, sLocs);
	request.setPath(path);
}
