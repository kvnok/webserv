#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"

bool pathExist(const string& path) {
    return filesystem::exists(path);
}

string create_file_name(string path) {
	size_t pos = path.find_last_of('=');
	string file = "";
	if (pos < path.size()) {
		file = path.substr(pos + 1);
		//cout << "File: " << file << endl;
	}
	cout << path << " vs " << file << "!" << endl;
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
	if (pos == -1)
		return 1;
	return 0;
}

void	checkDeletePath(Connection& connection) {
	return ;
}

void	deleteMethod(Connection& connection) {
	//should use status codes in these functions.
	string file = "";
	if (mustBeInStorage(connection.getRequest().getPath())) {
		connection.getRequest().setStatusCode(404);
		return ;
	}
	else
		file = "." + connection.getRequest().getPath();
	if (remove(file.c_str()) != 0)
		connection.getRequest().setStatusCode(404);
	else
		connection.getRequest().setStatusCode(204);
	// status code 204, is 'no content' so curl will not show anything.
}
