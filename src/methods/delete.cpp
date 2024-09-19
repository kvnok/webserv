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
		perror("Error deleting file");
	} else {
		cout << "File successfully deleted." << endl;
	}
}
/// kevini need you smart lock

void delete_method(int clientSocket, Request& request) {
	// Extract the requested path from the request
	string file = create_file_name(request.getBody());
	// cout << RED << "Requested path for deletion: " << file << RESET << endl;

	// I need it from here kevin kkrooooooon
	file = "www/storage/" + file;
	if (!pathExist(file)) {
		cout << BOLD << "File: " << file << " not found." << RESET << endl;
		createResponse(clientSocket, 200, "www/fileNotDeleted.html"); // Not Found
		return ;
	}
	else
	{
		// File successfully deleted
		cout << RED << "File: " << file << " deleted successfully." << RESET << endl;
		removeFileFromStorage(file); // Function to delete the file
		createResponse(clientSocket, 200, "www/fileDeleted.html"); // OK
		return ;
	}
}