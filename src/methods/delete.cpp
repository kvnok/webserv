#include "Connection.hpp"

void	deleteMethod(Connection& connection) {
	string file = connection.getRequest().getPath();
	if (!filesystem::exists(file))
		connection.getRequest().setStatusCode(404); //path doesnt exists.
	else if (!filesystem::is_regular_file(file))
		connection.getRequest().setStatusCode(403); //forbidden, is not a file
	else {
		filesystem::path parentPath = filesystem::path(file).parent_path();
		if (!filesystem::is_directory(parentPath) || access(parentPath.c_str(), W_OK) != 0)
			connection.getRequest().setStatusCode(403); //no writing rigths in the parent dir
		else {
			try {
				filesystem::remove(file);
				connection.getRequest().setStatusCode(204);
			}
			catch (const filesystem::filesystem_error&) {
				connection.getRequest().setStatusCode(500);
			}
		}
	}
	connection.setHandleStatusCode(true);
	connection.setNextState(STATUSCODE);
}
