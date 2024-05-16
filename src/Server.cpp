#include "../include/Server.hpp"

Server::Server( void )
{
	this->port = -1;
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
	int newSocket;
	struct sockaddr_in address;
	int opt = 1;
	int addrlen = sizeof(address);
	
	address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(this->port);

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
	if (bind(this->serverFd, (struct sockaddr *)&address, sizeof(address)) == -1) {
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
                std::cout << GREEN "Found server block" RESET << std::endl;
            }
            else if (firstWord(line) == "location")
            {
                std::cout << firstWord(line) << "Found location block" << std::endl;
            }
            else if (firstWord(line) == "listen")
            {
                //get the port number
                if (this->setPortNumberFromFile(line))
                    std::cout << GREEN "Found listen directive at port number: " << this->getPort() << RESET << std::endl;
            }
            else if (firstWord(line) == "server_name")
            {
                // parse server_name ||-- Remenber we can have multiple server_name --||
                if (this->setServerNameFromFile(line))
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

Server::~Server( void )
{

}
