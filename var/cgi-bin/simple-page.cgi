#include <iostream>

int main() {
    std::cout << "Content-type: text/html\n\n";
    std::cout << "<html>\n";
    std::cout << "<head>\n";
    std::cout << "<title>My First CGI</title>\n";
    std::cout << "</head>\n";
    std::cout << "<body>\n";
    std::cout << "<h1>Hello, world!</h1>\n";
    std::cout << "</body>\n";
    std::cout << "</html>\n";

    return 0;
}
