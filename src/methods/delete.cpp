#include "httpResponse.hpp"
#include "httpRequest.hpp"

bool pathExist(const string& path) {
    return filesystem::exists(path);
}

string create_file_name(string path) {
	cout << "Path: " << path << endl;
	size_t pos = path.find_last_of('=');
	cout << "Pos: " << pos << endl;
	string file = "";
	if (pos < path.size()) {
		file = path.substr(pos + 1);
		cout << "File: " << file << endl;
	}
	return file;
}

void removeFileFromStorage(const string& file) {
	if (remove(file.c_str()) != 0) {
		cerr << "Error deleting file" << endl;
	} else {
		cout << "File successfully deleted." << endl;
	}
}

int mustBeInStorage(const string& path) {
	int pos = path.find("storage");
	cout << "Pos: " << pos << endl;
	if (pos == -1)
		return 1;
	return 0;
}

void delete_method(Connection& connection, Request& request) {
	string file = "";
	if (request.getMethod() == "DELETE") {
		cout << RED << "Delete with curl not working" << RESET << endl;
		if (mustBeInStorage(request.getPath()))
		{
			cout << "File not in storage" << endl;
			request.setPath("www/fileNotDeleted.html");
			return ;
		}
		else
		{
			cout << "Delete method from curl" << endl;
			file = "." + request.getPath();
			request.setPath("www/fileDeleted.html");
		}
	}
	else
	{
		cout << "Delete method from Browser" << endl;
		file = create_file_name(request.getBody());
		file = "./www/storage/" + file;
		if (!pathExist(file)) {
			cout << BOLD << "File: " << file << " not found." << RESET << endl;
			request.setPath("www/fileNotDeleted.html");
			return ;
		}
	}
	cout << RED << "File: " << file << " deleted successfully." << RESET << endl;
	removeFileFromStorage(file); // Function to delete the file
	request.setPath("www/fileDeleted.html");
}