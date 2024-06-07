#include "autoindex.hpp"

// open the path of outfile.html and put it in browser to see the result
void test_autoindex() {
	cout << "-----------------\nTesting autoindex\n-----------------" << endl;
	vector<string> body;
	do_autoindex(body);

	// generate a file called outfile.html
	// send the body to outfile.html
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
