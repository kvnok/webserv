#include "Connection.hpp"

// set where the actualy file path of where we are going
// and set the status code for it


// set path to the relative path to the file or the correct error page
// if the path is not allowed, set the status code to 403
// if the path is not found, set the status code to 404

// check if they are asking for a file or a folder

static void parse_path(string &path, string &folder, string &file) {
	size_t pos = path.find_last_of('/');
	if (pos == string::npos) {
		folder = "/";
		file = path;
	}
	else {
		folder = path.substr(0, pos);
		file = path.substr(pos + 1);
	}
}

void request_path_handler(string &path, Request &request) {
	// first check if its just / or /file or /folder/
	string folder;
	string file;
	parse_path(path, folder, file);
	

	
	
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
/*
todo:
- read all the text
- what are the rules of the config loc path
- what are the rules of the request path
- when is the request path a file
- when is the request path a folder
- get the folder path from the request path
- get the file path from the request path
- compare the folder path with the loc path
- or check in / for the server baseline

- if the path matches something in the locs or baseline, then set path to the full file path
- if the path does not match, set the status code to 404 and the path to the 404 page from _error_pages

*/
/*
just set the status code
and the path
if statuscode changes, set the path to the error page
if statuscode is 200, set the path to the file
use _error_pages from server

*/
