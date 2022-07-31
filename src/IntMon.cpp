/* ╭──────────────────────────────────╮
*  │ Name: Kian Dadkhah Shokrollahi   │
*  │ SID: 119369205                   │
*  │ UNX511 Assignment 1              │
*  ╰──────────────────────────────────╯
*/

/* --- Network Interface Monitor: Interface ---
 * This unit contains the main function of the Interface Monitor portion of the assignment.
*/

// STL
#include <iostream>
#include <string>
#include <cstring>
#include <fstream>
// Linux Libraries
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/un.h>
#include <net/if.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
// My Libraries
#include "utils.cpp"

int main(int argc, char *argv[]) {
  std::string data;
  bool is_running = true;
	int ret_val = 0;
	char interface[MAX_BUFFER];
	strncpy(interface, argv[1], MAX_BUFFER);

	//Set up socket communications
	struct sockaddr_un addr;
	char buffer[BUFFER_SIZE];
	int len, ret, fd, rc;

	std::cout << "Client " << getpid() << ": Running..." << std::endl;
	memset(&addr, 0, sizeof(addr));
	//Create the socket
	if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
	{
		std::cout << "Client " << getpid() << ", Error: " << strerror(errno) << std::endl;
		exit(-1);
	}

	addr.sun_family = AF_UNIX;
	//Set the socket path to a local socket file
	strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

	//Connect to the local socket
	if (connect(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		std::cout << "Client " << getpid() << ", Error: " << strerror(errno) << std::endl;
		close(fd);
		exit(-1);
	}

	len = sprintf(buffer, "Ready");
	bzero(buffer, BUFFER_SIZE);
	ret = write(fd, buffer, BUFFER_SIZE);
	ret = read(fd, buffer, BUFFER_SIZE); //Read something from the server
	if (ret < 0)
	{
		std::cout << "Client " << getpid() << ": error reading socket." << std::endl;
		std::cout << strerror(errno) << std::endl;
	}

	std::cout << "Received message from Network Monitor: " << buffer << std::endl;
	while (is_running)
	{
		if (strcmp(buffer, "Quit") == 0)
		{ //Server requests the client to terminate
			std::cout << "client(" << getpid() << "): received request to quit" << std::endl;
			is_running = false;
		}

		//gets all the required interface info and stores them in 'data' variable
		nimutils::getInterfaceStat(interface, data);

		bzero(buffer, BUFFER_SIZE);
		strcpy(buffer, data.c_str());
		std::cout << buffer;
		buffer[len] = '\0';
		ret = write(fd, buffer, BUFFER_SIZE); //The client writes the data
		if (ret == -1)
		{
			std::cout << "Client: data Write Error" << std::endl;
			std::cout << strerror(errno) << std::endl;
		}
		sleep(1);
	}

	return 0;
}
