#include "Connection.hpp"
#include "httpRequest.hpp"
#include "httpResponse.hpp"
#include "httpStatus.hpp"

Connection::Connection( void ) { }

Connection::Connection( vector<Server> &servers) {
	this->server = servers;
	this->setFds();
}

void Connection::setFds() {
	for (int i = 0; i < this->server.size(); i++) {
		pollfd fd;
		fd.fd = this->server[i].getFd();
		fd.events = POLLIN;
		this->fds.push_back(fd);
	}
}

void Connection::handleNewConnection(int i)
{
    int clientSocket = accept(this->server[i].getFd(), NULL, NULL);
    if (clientSocket == -1)
    {
        cerr << "accept failed" << endl;
        // we can also add an exception or send the client to a page with a status code and a message
        return;
    }
    this->fds.push_back({clientSocket, POLLIN});
}

// add in response utils
void checkAndSetContentTypeExtesion(string header, Response &response)
{
    string extension = header.substr(header.find_last_of(".") + 1);
    if (extension == "html")
        response.addHeader("Content-Type", "text/html");
    else if (extension == "css")
        response.addHeader("Content-Type", "text/css");
    else if (extension == "ico")
        response.addHeader("Content-Type", "image/x-icon");

}

void Connection::handleExistingConnection(int i)
{
    vector<char> buffer(4092); // max size of request to fix Maybe we can use Max body size and then resize;
    int clientSocket = this->fds[i].fd;
    ssize_t bytes = recv(clientSocket, buffer.data(), buffer.size(), 0);
    if (bytes < 0)
    {
        cerr << "recv failed" << endl;
        return;
    }
    else if ( bytes == 0 )
    {
        cout << "Connection closed" << clientSocket << endl;
        close(clientSocket);
        this->fds.erase(this->fds.begin() + i);
        return;
    }
    // if there is a max buffer size we can check here
    buffer.resize(bytes);
	// here is where Jangijs magic happens
    Request request;
    readRequest(buffer.data(), request);
    Response response(request);
    // in Resoponse class not here //
    checkAndSetContentTypeExtesion(request.getPath(), response);
    handleRequest(clientSocket, request, response, i); // Jangijs magic
}

// bool pathIsLocation(string path, vector<Location> location)
// {
//     for (int i = 0; i < location.size(); i++)
//     {
//         if (location[i].get_path() == path)
//             return true;
//     }
//     return false;
// }

// in Resoponse class not here //
void Connection::handleRequest(int clientSocket, Request& request, Response& responseClass, int i)
{
    /* Response schema
    |  Version StatusCode StatusMessage     ||-- We have that in the response class --||
    |  Header                               ||-- We have that in the response class --||
 -------   ||||||| Possible we need to add more things to the header ||||||| --------
    |  "Content-Type: " create a funtion that returns the content type based on the file extension
    |  "Content-Length: " + content size() + "\r\n\r\n"
    |  Content  
    */
// if cgi we can call the cgi class
    // if simple request we can call the request class
    // here we can split if cgi or simple request
    // if cgi we can call the cgi class
    // if simple request we can call the request class
    string path;
    // if (pathIsLocation(request.getPath(), this->server[i].getLocations()))
    // {
    //     cout << "location found" << endl;
    //     // allowed methods change if we have to ch
    // }
    // else
    //     cout << "location not found" << endl;
    if (request.getPath() == "/") 
    path = "www/index.html";
    else
        path = "www" + request.getPath();
    if (request.getStatusCode() != 200)
        path = getHtmlPath(request.getStatusCode());
    ifstream file(path);
    string content;
    if (file)
        content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    else {
        cerr << "Failed to open file: " << path << endl;
        request.setStatusCode(404);
        path = getHtmlPath(request.getStatusCode());
        ifstream file(path);
        content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    }
    responseClass.setBody(content);
    responseClass.addHeader("Content-Length", to_string(content.size()));
    string response = responseClass.createResponseString();
    send(clientSocket, response.c_str() , response.size(), 0);
}

void Connection::start()
{
    for (int i = 0; i < this->server.size(); i++)
        cout << "Listening at port: " << this->server[i].getPort() << endl;
    while (true)
    {
        int ret = poll(this->fds.data(), this->fds.size(), 0);
        if (ret == -1)
            throw runtime_error("poll failed");
        for (int i = 0; i < this->fds.size(); i++)
        {
            if (this->fds[i].revents & POLLIN)
            {
                if (i < this->server.size() && this->fds[i].fd == this->server[i].getFd())
                    handleNewConnection(i);
                else
                    handleExistingConnection(i);
            }
        }
    }
}

Connection::~Connection( void ) { }
