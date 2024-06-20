#include "Connection.hpp"

// set where the actualy file path of where we are going
// and set the status code for it


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

/*
rules:
the path from the request should end with a / if its just asking a location
if its asking for a file, it should not end with a /

the location route path cannot have a . in it


(optional) if we are looking for /file and there is no location route of /file, then search for file in server root
*/
