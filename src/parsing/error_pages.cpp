#include "Parser.hpp"

static const vector<int> all_error_codes = {
100, 101, 102, 103, 200, 201, 202, 203, 204, 205, 206, 207, 208, 226, 300, 301, 302, 303, 304, 305, 306, 307, 308, 400, 401, 402, 403, 404, 405, 406, 407, 408, 409, 410, 411, 412, 413, 414, 415, 416, 417, 418, 421, 422, 423, 424, 425, 426, 428, 429, 431, 451, 500, 501, 502, 503, 504, 505, 506, 507, 508, 510, 511
};

void fill_error_pages(pServerBlock &block) {
	map<int, string> set_pages = block.get_error_pages();
	// print set pages
	// cout << "set pages" << endl;
	// for (map<int, string>::iterator it = set_pages.begin(); it != set_pages.end(); it++) {
	// 	cout << it->first << " " << it->second << endl;
	// }
	map<int, string> error_pages;
	for (int i = 0; i < (int)all_error_codes.size(); i++) { //CHANGED cast to int
		if (set_pages.find(all_error_codes[i]) != set_pages.end()) {
			error_pages[all_error_codes[i]] = set_pages[all_error_codes[i]];
		}
		else {
			error_pages[all_error_codes[i]] = "./var/error/" + to_string(all_error_codes[i]) + ".html";
		}
	}
	for (map<int, string>::iterator it = error_pages.begin(); it != error_pages.end(); it++) {
		if (set_pages.find(it->first) == set_pages.end()) {
			block.add_error_page(it->first, it->second);
		}
	}
	block.set_error_pages(error_pages);
}

/*
100: "Continue",
101: "Switching protocols",
102: "Processing",
103: "Early Hints",
200: "OK",
201: "Created",
202: "Accepted",
203: "Non-Authoritative Information",
204: "No Content",
205: "Reset Content",
206: "Partial Content",
207: "Multi-Status",
208: "Already Reported",
226: "IM Used",
300: "Multiple Choices",
301: "Moved Permanently",
302: "Found (Previously \"Moved Temporarily\")",
303: "See Other",
304: "Not Modified",
305: "Use Proxy",
306: "Switch Proxy",
307: "Temporary Redirect",
308: "Permanent Redirect",
400: "Bad Request",
401: "Unauthorized",
402: "Payment Required",
403: "Forbidden",
404: "Not Found",
405: "Method Not Allowed",
406: "Not Acceptable",
407: "Proxy Authentication Required",
408: "Request Timeout",
409: "Conflict",
410: "Gone",
411: "Length Required",
412: "Precondition Failed",
413: "Payload Too Large",
414: "URI Too Long",
415: "Unsupported Media Type",
416: "Range Not Satisfiable",
417: "Expectation Failed",
418: "I'm a Teapot",
421: "Misdirected Request",
422: "Unprocessable Entity",
423: "Locked",
424: "Failed Dependency",
425: "Too Early",
426: "Upgrade Required",
428: "Precondition Required",
429: "Too Many Requests",
431: "Request Header Fields Too Large",
451: "Unavailable For Legal Reasons",
500: "Internal Server Error",
501: "Not Implemented",
502: "Bad Gateway",
503: "Service Unavailable",
504: "Gateway Timeout",
505: "HTTP Version Not Supported",
506: "Variant Also Negotiates",
507: "Insufficient Storage",
508: "Loop Detected",
510: "Not Extended",
511: "Network Authentication Required"
*/
