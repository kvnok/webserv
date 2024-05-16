#include "../include/Server.hpp"

int configFileParser(std::string line)
{
	std::cout << line << std::endl; 
	return (1);
}

void handleConfigFile(std::string configFile)
{
    Server Server;
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
                if (Server.setPortNumberFromFile(line))
                    std::cout << GREEN "Found listen directive at port number: " << Server.getPort() << RESET << std::endl;
            }
            else if (firstWord(line) == "server_name")
            {
                // parse server_name ||-- Remenber we can have multiple server_name --||
                if (Server.setServerNameFromFile(line))
                    std::cout << GREEN << firstWord(line) << "Found server_name: " << Server.getServerName()[0] << RESET << std::endl;
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
    if (Server.getPort() == -1)
    {
        Server.setPortNumberFromFile("");
    }
    Server.getServerName();
}
