#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"

// string create_file_name(string path) {
// 	size_t pos = path.find_last_of('=');
// 	string file = "";
// 	if (pos < path.size()) {
// 		file = path.substr(pos + 1);
// 		//cout << "File: " << file << endl;
// 	}
// 	cout << path << " vs " << file << "!" << endl;
// 	return file;
// }

// void removeFileFromStorage(const string& file) {
// 	if (remove(file.c_str()) != 0) {
// 		cerr << "Error deleting file" << endl;
// 	} else {
// 		//cout << "File successfully deleted." << endl;
// 	}
// }

void	deleteMethod(Connection& connection) {
	string file = connection.getRequest().getPath();
	if (!filesystem::exists(file)) {
		connection.getRequest().setStatusCode(404);
		return ;
	}
	//we should assume that all check have been done in the path_handler.
	//so the path we have here, should be of a file in a folder where we are allowed to delete, (config file should allow DELETE, or we should create a location rule for delete)
	if (remove(file.c_str()) != 0)
		connection.getRequest().setStatusCode(404);
	else
		connection.getRequest().setStatusCode(204);
	// status code 204, is 'no content' so curl will not show anything.
}
