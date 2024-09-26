#pragma once

#include "stuff.hpp"
#include <chrono>
#include <filesystem>

namespace fs = filesystem;

bool is_file(const string& path);
bool is_directory(const string& path);
string do_autoindex(const string& path);
