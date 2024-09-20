#include "autoindex.hpp"

bool is_file(const string& path) {
    return fs::exists(path) && fs::is_regular_file(path);
}

bool is_directory(const string& path) {
    return fs::exists(path) && fs::is_directory(path);
}
