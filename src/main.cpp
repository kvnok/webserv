#include "../include/stuff.hpp"

int main(int argc, char **argv)
{
	
	return 0;
}

/*
- startup setup
cmd input conf or use default conf
then use the parser to look at all the things in conf
and check if something doesnt belong
save all the different directives
then we have all the conf server info

parsing is checking
config class is setting

set the server class with the info from the config class
in the server class we use socket, bind, listen

-in a loop
then we have the poll loop in which we use accept
on the server socket from the server class
and we check all the status of the client sockets
then we read the request
parse the request in general and against the config file
then we generate the appropriate response
and send it back to the client


*/

/*
we start with dummy data
expect that the requests are basic
and the config file is correct

so we need to create the infrastructure for where the dummy data is stored
which is in the config class

*/
1