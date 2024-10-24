#include "httpResponse.hpp"
#include "httpRequest.hpp"
#include "Connection.hpp"

static string extract_file_name(const string &path) {
    size_t posSlash = path.find_last_of('/');
    string file;
    if (posSlash == string::npos)
        return nullptr;
    else
        file = path.substr(posSlash + 1);
    return file;
}


bool check_is_CGI(Connection &connection) {
	string name = extract_file_name(connection.getRequest().getPath());
	if (name.find(".cgi") != string::npos)
	{
		cout << "CGI" << endl;
		return true;
	}
	return false;
}