#include "httpResponse.hpp"

void close_uploaded_image_page(int clientSocket)
{
	const string imagePath = to_string(clientSocket) + "uploadedImage.html";
	remove(imagePath.c_str());
}

void	delete_method( int clientSocket, Request& request, ifstream& file)
{
	cout << "Method: " << request.getMethod() << endl;
}