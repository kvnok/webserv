#include "httpRequest.hpp"
#include "Connection.hpp"

/*
new set up:

we go here if it is delete,
we delete the file
and we set the status code to 'succes' or 'not succes' and we go on in the statusCodePage function.
*/

void	deleteMethod(Connection& connection) {
	string file = connection.getRequest().getPath();
	if (!filesystem::exists(file))
		connection.getRequest().setStatusCode(404);
	else {
		if (remove(file.c_str()) != 0)
			connection.getRequest().setStatusCode(404);
		else
			connection.getRequest().setStatusCode(204);
	}
	connection.setHandleStatusCode(true);
	connection.setNextState(STATUSCODE);
}
