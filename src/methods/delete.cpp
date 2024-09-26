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
/// kevini need you smart lock

void delete_method(int clientSocket, Request& request) {
	// Extract the requested path from the request
	string file = create_file_name(request.getBody());

	// I need it from here kevin kkrooooooon
	file = "www/storage/" + file;
	if (!pathExist(file)) {
		cout << BOLD << "File: " << file << " not found." << RESET << endl;
		request.setPath("www/fileNotDeleted.html");
		return ;
	}
	else
	{
		// File successfully deleted
		if (request.getBody().empty())
			cout << RED << "File: " << file << "Delete with curl not working" << RESET << endl;
		else
		{
			cout << RED << "File: " << file << " deleted successfully." << RESET << endl;
			removeFileFromStorage(file); // Function to delete the file
			request.setPath("www/fileDeleted.html");
		}
	}
}