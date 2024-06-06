#include "CGI.hpp"

int test_cgi(string cgi_path) {
	cout << GRN << "Testing for " << cgi_path << RESET << endl;
	string ret = "";
	string status = "succes";
	do_cgi(ret, cgi_path, status);
	cout << RED << "Status: |" << status << "|" << RESET << endl;
	cout << BLU << "Output: |" << ret << "|\n" << RESET << endl;
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
	test_cgi(cgi_path + "blabla.cgi");
	return 0;
}
