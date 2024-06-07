#include "autoindex.hpp"


static void print_vector(vector<string> &body) {
	for (vector<string>::iterator it = body.begin(); it != body.end(); it++) {
		cout << *it << endl;
	}
}

// open the path of outfile.html and put it in browser to see the result
void test_autoindex() {
	cout << "-----------------\nTesting autoindex\n-----------------" << endl;
	vector<string> body;
	body.push_back("<!DOCTYPE html>");
	body.push_back("<html lang=\"en\">");
	body.push_back("<head>");
	body.push_back("<meta charset=\"UTF-8\">");
	body.push_back("<meta http-equiv=\"X-UA-Compatible\" content=\"IE=edge\">");
	body.push_back("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
	body.push_back("<title>Autoindex</title>");
	body.push_back("</head>");
	body.push_back("<body>");
	body.push_back("<hr>");
	do_autoindex(body);
	body.push_back("<hr>");
	body.push_back("</body>");
	body.push_back("</html>");
	// generate a file called outfile.html
	// send the body to outfile.html
	print_vector(body);
	// ofstream outfile("outfile.html");
	// for (vector<string>::iterator it = body.begin(); it != body.end(); it++) {
	// 	outfile << *it << endl;
	// }
	// outfile.close();
}

/*
index of location path
<table>
	<tr>
		<td>name</td>
		<td>last modified</td>
		<td>size</td>
	</tr>
	<tr>
		<td>file1</td>
		<td>date</td>
		<td>size</td>
	</tr>
	<tr>
		<td>file2</td>
		<td>date</td>
		<td>size</td>
	</tr>
	<tr>
		<td>file3</td>
		<td>date</td>
		<td>size</td>
	</tr>
</table>
*/
