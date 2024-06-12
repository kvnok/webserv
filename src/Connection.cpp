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

void Connection::start() {
	for (int i = 0; i < this->server.size(); i++)
		cout << "Listening at port: " << this->server[i].getPort() << endl;
	while (true) {
		int ret = poll(this->fds.data(), this->fds.size(), 0);
		if (ret == -1)
			cerr << "poll failed" << endl;
		for (int i = 0; i < this->fds.size(); i++) {
			char buffer[1024] = {0};
			if (this->fds[i].revents & POLLIN) {
				if (this->fds[i].fd == this->server[i].getFd()) {
					int clientSocket = accept(this->server[i].getFd(), NULL, NULL);
					if (clientSocket == -1) {
						cerr << "accept failed" << endl;
						continue ;
					}
					else
						this->fds.push_back({clientSocket, POLLIN});
				}
				else {
					int clientSocket = this->fds[i].fd;
					ssize_t bytes = recv(clientSocket, buffer, sizeof(buffer), 0);
					if (bytes < 0)
						cerr << "Failed to receive data: " << strerror(errno) << endl;
					else if ( bytes == 0 )
						cout << "connection closed" << endl;
					else
					{
						cout << "Received message from client: " << buffer << std::endl;
						Request request;
						readRequest(buffer, request);
						Response response(request);
						string content = "";
						if (request.getStatusCode() == 200) {
							if (request.getPath() == "/") {
								string path = "www/index.html";
								ifstream file(path);
								if (file) 
									content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
								else
									cerr << "Failed to open file: " << path << endl;
								response.addHeader("Content-Type", "text/html");
							}
						}
						else {
							string path = getHtmlPath(response.getStatusCode());
							ifstream file(path);
							if (file) 
								content = string ((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
							else
								cerr << "Failed to open file: " << path << endl;
							response.addHeader("Content-Type", getHtmlPath(response.getStatusCode()));
						}
						response.addHeader("Content-Length", to_string(content.size()));
						response.setBody(content);
						string responseString = response.createResponseString();
						send(clientSocket, responseString.c_str(), responseString.size(), 0);
					}
				}
			}
		}
	}
}

Connection::~Connection( void ) { }
