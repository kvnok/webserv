#include "Connection.hpp"


void request_path_handler(string &path, Request &request) {
	// set path to the relative path to the file or the correct error page
	// if the path is not allowed, set the status code to 403
	// if the path is not found, set the status code to 404
	
	smartLocs sLocs;
	sLocs.set_locs(request.getServer().getSmartLocs().get_locs());
	path = "www" + request.getPath();
}
