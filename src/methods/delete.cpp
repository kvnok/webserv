#include "httpRequest.hpp"
#include "Connection.hpp"

void	deleteMethod(Connection& connection) {
	string file = connection.getRequest().getPath();
	if (!filesystem::exists(file)) {
		connection.getRequest().setStatusCode(404);
		return ;
	}
	else {
		if (remove(file.c_str()) != 0)
			connection.getRequest().setStatusCode(404);
		else
			connection.getRequest().setStatusCode(204);
	}
	connection.setNextState(STATUSCODE);
}
