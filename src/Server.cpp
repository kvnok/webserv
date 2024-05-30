#include "../include/Server.hpp"

Server::Server( void )
{
	this->port = -1;
    this->serverFd = 0;
    // this->maxClients = -1;
}

Server::Server( int port)
{
	this->port = port;
	this->server_name = std::vector<std::string>();
}

void Server::setPort(int port)
{
	this->port = port;
}

bool Server::setPortNumberFromFile(std::string line)
{
	size_t i = skipFirstWord(line);
	if(line.empty() || containsOnlyNumbers(line.substr(i)) == false)
	{
		this->port = 8080;
		if (containsOnlyNumbers(line) == false)
			std::cout << RED "Port contains digit" RESET << std::endl;
		std::cout << GREEN "No port number found, defaulting to 8080" RESET << std::endl;
		return (false);
	}
	else
		this->port = atoi(line.substr(i).c_str());
	return (true);
}

bool Server::setServerNameFromFile(std::string line)
{
    // returns the count of server names
    int countServerName = howManyServerName(line);
	// printf("countServerName: %d\n", countServerName);
    
    size_t i = skipFirstWord(line) -1;
    for (int e = 0; e < countServerName; e++)
	{
        // Extract each server name and add it to the vector
		line = skipWhiteSpace(line);
        size_t spacePos = line.find(' ', i); // Find the position of the next space
        std::string serverName = line.substr(i, spacePos - i); // Extract the server name
        this->server_name.push_back(serverName); // Add the server name to the vector
        i = spacePos + 1; // Move to the next character after the space
    }
    return true;
}

void Server::setSocket()
{
	this->opt = 1;
	this->addrlen = sizeof(this->address);
	
	this->address.sin_family = AF_INET;
    this->address.sin_addr.s_addr = INADDR_ANY;
    this->address.sin_port = htons(this->port);

	if ((this->serverFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(this->serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) == -1)
	{
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
	if (bind(this->serverFd, (struct sockaddr *)&this->address, sizeof(this->address)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(this->serverFd, MAX_CLIENTS) == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
}

int Server::getPort()
{
	return (this->port);
}

std::vector<std::string> Server::getServerName()
{
	// for (size_t i = 0; i < this->server_name.size(); i++){
	// 	std::cout << "|" << skipWhiteSpace(this->server_name[i]) << "|" << std::endl;
	// }
	return (this->server_name);
}

void Server::handleConfigFile(std::string configFile)
{
    std::cout << "Processing configuration from file: " << configFile << std::endl;
    // Example: Read and print contents of configuration file
    std::ifstream file(configFile);
    if (file.is_open()) 
    {
        std::string line;
        while (getline(file, line))
        {
            if (firstWord(line) == "server")
            {
                // std::cout << GREEN "Found server block" RESET << std::endl;
            }
            else if (firstWord(line) == "location")
            {
                // std::cout << firstWord(line) << "Found location block" << std::endl;
            }
            else if (firstWord(line) == "listen")
            {
                this->setPortNumberFromFile(line);
                //get the port number
                // if (this->setPortNumberFromFile(line))
                    // std::cout << GREEN "Found listen directive at port number: " << this->getPort() << RESET << std::endl;
            }
            else if (firstWord(line) == "server_name")
            {
                // parse server_name ||-- Remenber we can have multiple server_name --||
                this->setServerNameFromFile(line);
                // if (this->setServerNameFromFile(line))
                    std::cout << GREEN << firstWord(line) << "Found server_name: " << this->getServerName()[0] << RESET << std::endl;
            }
            // else if (firstWord(line) == "root")
            // {
            //     std::cout << firstWord(line) << "Found root directive" << std::endl;
            // }
            // else if (firstWord(line) == "index")
            // {
            //     std::cout << firstWord(line) << "Found index directive" << std::endl;
            // }
            // else if (firstWord(line) == "error_page")
            // {
            //     std::cout << firstWord(line) << "Found error_page directive" << std::endl;
            // }
        }
        file.close();
    }
	else
    {
        std::cerr << "Unable to open configuration file: " << configFile << std::endl;
    }
    if (this->getPort() == -1)
    {
        this->setPortNumberFromFile("");
    }
    this->getServerName();
}

// void Server::serveHtml(Server& server)
// {
//     std::string filename = (this->path == "/") ? "www/index.html" : "www/" + this->path;
//     std::ifstream html_file(filename);
//     static bool error_printed = false; // Flag to track if error message has been printed

//     // Reset the error_printed flag if the file is found

//     // File exists, serve it
//     std::stringstream response;
//     response << "HTTP/1.1 200 OK\r\n";
//     response << "Content-Type: text/html\r\n";
//     response << "Connection: keep-alive\r\n"; // Keep the connection alive
//     response << "\r\n";
//     response << html_file.rdbuf();

//     std::string response_str = response.str();
//     send(this->newSocket, response_str.c_str(), response_str.length(), 0);
// }

void Server::getClient(int client_fd)
{

}

void Server::parseRequestPath(std::string request)
{
    std::istringstream iss(request);
    std::string request_line;
    std::getline(iss, request_line);
    std::istringstream iss_line(request_line);
    std::string method , path, protocol;
    iss_line >> method >> path >> protocol;
    this->path = path;
}

void Server::serveHtml(Server& server)
{
    std::string filename;
    if (this->path == "/")
        filename = "www/index.html";
    else
        filename = "www/" + this->path;
    std::ifstream html_file(filename);
    std::ifstream css_file(filename);
    std::stringstream response;
    if (css_file.good())
    {
        std::string content((std::istreambuf_iterator<char>(css_file)), std::istreambuf_iterator<char>());

        // std::stringstream response;
        response << "HTTP/1.1 200 OK\r\n";
        response << "Content-Type: text/css\r\n";
        response << "Content-Length: " << content.size() << "\r\n";
        // response << "Connection: close\r\n"; // Close the connection after sending the response
        response << "\r\n";
        response << content;
        std::cout << "CSS file found" << std::endl;
    }
    // if (!html_file.good()) {
    //     std::cerr << "Failed to open file: " << filename << std::endl;
    //     return;
    // }
    // else
    // {
        // std::string content((std::istreambuf_iterator<char>(html_file)), std::istreambuf_iterator<char>());

        // response << "HTTP/1.1 200 OK\r\n";
        // response << "Content-Type: text/html\r\n";
        // response << "Content-Length: " << content.size() << "\r\n";
        // // response << "Connection: close\r\n"; // Close the connection after sending the response
        // response << "\r\n";
        // response << content;
    // }
    // std::string content((std::istreambuf_iterator<char>(html_file)), std::istreambuf_iterator<char>());

    // std::stringstream response;
    // response << "HTTP/1.1 200 OK\r\n";
    // response << "Content-Type: text/html\r\n";
    // response << "Content-Length: " << content.size() << "\r\n";
    // response << "Connection: close\r\n"; // Close the connection after sending the response
    // response << "\r\n";
    // response << content;

    std::string response_str = response.str();
    printf("Response: %s\n", response_str.c_str());

    ssize_t bytes_sent = send(this->newSocket, response_str.c_str(), response_str.length(), 0);
    if (bytes_sent < 0) {
        std::cerr << "Failed to send response: " << strerror(errno) << std::endl;
    }
}

int Server::connect( void )
{
    char buffer[BUFFER_SIZE] = {0};
    this->setSocket();
    while (true)
    {
        if ((this->newSocket = accept(this->serverFd, (struct sockaddr *)&this->address, (socklen_t*)&this->addrlen)) == -1) {
            perror("accept failed");
            continue;
        }

        // Receive HTTP request
        ssize_t bytes_received = recv(this->newSocket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            // Handle error or closed connection
            close(this->newSocket);
            continue;
        }

        std::string request(buffer, bytes_received);

        // Parse request path
        parseRequestPath(request);
        
        // Serve HTML file
        serveHtml(*this);
        // Close the connection
        close(this->newSocket);
    }

    close(this->serverFd);
    return (0);
}

Server::~Server( void )
{

}
