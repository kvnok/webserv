#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <exception>
#include <iomanip>
#include <cmath>
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <functional>
#include <iterator>
#include <utility>
#include <cstdlib>
#include <map>
#include <vector>
#include <list>
#include <set>
#include <stack>
#include <queue>
#include <deque>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <sys/time.h>
#include <sys/select.h>
#include <cstring>
#include <signal.h>
#include <sys/wait.h>
#include <filesystem>

using namespace std;

// ANSI escape codes for text color
#define RED "\033[1;31m"
#define GRN "\033[1;32m"
#define YEL "\033[1;33m"
#define BLU "\033[1;34m"
#define MAG "\033[1;35m"
#define CYN "\033[1;36m"
#define WHT "\033[1;37m"
#define RESET "\033[0m"

#define BOLD "\033[1m"
#define UNDERLINE "\033[4m"
#define REVERSED "\033[7m"

// Print colored text
template<typename... Args>
void color_print(string color, Args... args) {
	cout << color;
	(cout << ... << args);
	cout << RESET << endl;
}
