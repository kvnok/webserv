#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"

bool pathExist(const string& path) {
    return filesystem::exists(path);
}

string create_file_name(string path) {
	//cout << "Path: " << path << endl;
	size_t pos = path.find_last_of('=');
	//cout << "Pos: " << pos << endl;
	string file = "";
	if (pos < path.size()) {
		file = path.substr(pos + 1);
		//cout << "File: " << file << endl;
	}
	return file;
}

void removeFileFromStorage(const string& file) {
	if (remove(file.c_str()) != 0) {
		cerr << "Error deleting file" << endl;
	} else {
		//cout << "File successfully deleted." << endl;
	}
}

int mustBeInStorage(const string& path) {
	int pos = path.find("storage");
	//cout << "Pos: " << pos << endl;
	if (pos == -1)
		return 1;
	return 0;
}

void deleteMethod(Connection& connection) {
	//should use status codes in these functions.
	string file = "";
	if (mustBeInStorage(connection.getRequest().getPath()))
	{
		//cout << "File not in storage" << endl;
		connection.getRequest().setStatusCode(404);
		return ;
	}
	else
	{
		//cout << "Delete method from curl" << endl;
		file = "." + connection.getRequest().getPath();
	}
	//cout << RED << "File: " << file << " deleted successfully." << RESET << endl;
	removeFileFromStorage(file); // Function to delete the file
	map<int, string> err_pages = connection.getServer().getErrorPages();
	int statusCode = connection.getRequest().getStatusCode();
	connection.getRequest().setPath(err_pages[statusCode]);
}
