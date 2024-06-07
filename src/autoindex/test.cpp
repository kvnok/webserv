#include "autoindex.hpp"

// open the path of outfile.html and put it in browser to see the result
void test_autoindex() {
	cout << "-----------------\nTesting autoindex\n-----------------" << endl;
	string body = "";
	do_autoindex(body);

	// generate a file called outfile.html
	// send the body to outfile.html
}
