#include "CGI.hpp"

int test_cgi(string cgi_path) {
	cout << GRN << "Testing for " << cgi_path << RESET << endl;
	string ret = "";
	if (access(cgi_path.c_str(), F_OK) == -1) {
		cout << "The file does not exist." << endl;
		return 1;
	}

	string err = do_cgi(ret, cgi_path);
	cout << RED << "Error: |" << err << "|" << RESET << endl;
	cout << BLU << "Output: |" << ret << "|" << RESET << endl;
	return 0;
}

int cgi_multiple_tests() {
	cout << "-----------------\nTesting CGI\n-----------------" << endl;
	string cgi_path = "./var/cgi-bin/";
	test_cgi(cgi_path + "page.cgi");
	test_cgi(cgi_path + "get_time.cgi");
	// test_cgi(cgi_path + "upload.cgi");
	// test_cgi(cgi_path + "form_processing.cgi");
	test_cgi(cgi_path + "enviroment.cgi");
	test_cgi(cgi_path + "server_info.cgi");
	test_cgi(cgi_path + "json_response.cgi");
	return 0;
}
