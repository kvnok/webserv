#include "Connection.hpp"


// set path to the relative path to the file or the correct error page
// if the path is not allowed, set the status code to 403
// if the path is not found, set the status code to 404

// check if they are asking for a file or a folder

void request_path_handler(string &path, Request &request) {
	// first check if its just / or /file or /folder/

	
	
	smartLocs sLocs;
	sLocs.set_locs(request.getServer().getSmartLocs().get_locs());
	// path = "www" + request.getPath();
	// get only the dir path
}

/*
/www/index.html -> /www
/www/ -> /www
/ -> /
/bla/bla/bla -> /bla/bla
/test/one.html -> /test
*/