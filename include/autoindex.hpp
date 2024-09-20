#pragma once

#include "stuff.hpp"

void test_autoindex();
void do_autoindex(vector<string> &body);

namespace fs = filesystem;

// Function declarations
bool is_file(const string& path);
bool is_directory(const string& path);
