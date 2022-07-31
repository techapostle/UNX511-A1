/* ╭──────────────────────────────────╮
*  │ Name: Kian Dadkhah Shokrollahi   │
*  │ SID: 119369205                   │
*  │ UNX511 Assignment 1              │
*  ╰──────────────────────────────────╯
*/

/* --- Network Interface Monitor: Network ---
 * This unit contains the main function of the Network Monitor portion of the assignment.
*/

// STL
#include <cstdio>
#include <iostream>
#include <string>
// Linux Libraries
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
// Project Libraries
#include "utils.cpp"

int main() {
  struct sockaddr_un addr; // Create a socket for inter-process communication.
  int numInterfaces = 1; // Default number of interfaces
  int numClients = 0;
  std::string *interfaces = nullptr; // String holding names of interfaces
  char buffer[BUFFER_SIZE];
  int clients[MAX_CLIENTS];
  int len, master_fd, max_fd, rc, ret;
  fd_set active_fd_set;
  fd_set read_fd_set;

  // Signal Handling
  struct sigaction action;
  action.sa_handler = nimutils::signalHandler;
  sigemptyset(&action.sa_mask);
  action.sa_flags = 0;
  sigaction(SIGINT, &action, NULL);

  // Socket handling
  memset(&addr, 0, sizeof(addr));
  master_fd = socket(AF_UNIX, SOCK_STREAM, 0);
  if (master_fd < 0) {
    std::cout << "Server: " << strerror(errno) << std::endl;
    exit(-1);
  }
  addr.sun_family = AF_UNIX;
  strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1); // Set socket path as defined in socket_path
  unlink(SOCKET_PATH);
  if (bind(master_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
    std::cout << "Server: " << strerror(errno) << std::endl;
    close(master_fd);
    exit(-1);
  }

  std::cout << "How many interfaces do you wish to monitor?" << std::endl;
  std::cin >> numInterfaces;
  interfaces = new std::string[numInterfaces];
  for (int i = 0; i < numInterfaces; i++) {
    std::cout << "\nInterface " << i+1 << " name: ";
    std::cin >> interfaces[i];
  }

  if (fork() == 0) {
    for (int i = 0; i < numInterfaces; i++) {
      if (fork() == 0) {
        std::cout << interfaces[i] << std::endl;
        execlp("./interfaceMon", "./interfaceMon", interfaces[i].c_str(), NULL);
      }
    }
  } else {
    std::cout << "Waiting for the client to connect..." << std::endl;
    // Listen for client to connect to the socket file
    if (listen(master_fd, 5) == -1) {
      std::cout << "Network Monitor: " << strerror(errno) << std::endl;
      unlink(SOCKET_PATH);
      close(master_fd);
      exit(-1);
    } else {
      FD_ZERO(&read_fd_set);
      FD_ZERO(&active_fd_set);
      FD_SET(master_fd, &active_fd_set); // Add master_fd to socket set
      max_fd = master_fd; // Select from max_fd sockets
      nimutils::terminate_NM = false;
      while (!nimutils::terminate_NM) {
        read_fd_set = active_fd_set; // Block until an input arrives on a socket
        ret = select(max_fd+1, &read_fd_set, NULL, NULL, NULL); // Select from up to max_fd+1 sockets
        if (ret < 0) {
          std::cout << "Network Monitor: " << strerror(errno) << std::endl;
          // nimutils::terminate = true;
        } else {
          if (FD_ISSET(master_fd, &read_fd_set)) {
            clients[numClients] = accept(master_fd, NULL, NULL);
            if (clients[numClients] < 0) {
              std::cout << "Network Monitor: " << strerror(errno) << std::endl;
              // nimutils::terminate = true;
            } else {
              std::cout << "Network Monitor: incoming connection "
                << clients[numClients] << std::endl;
              ret = read(clients[numClients], buffer, BUFFER_SIZE);
              std::cout << "Received message from Interface Monitor: " << buffer << std::endl;
              len = sprintf(buffer, "Monitor") + 1;
              ret = write(clients[numClients], buffer, BUFFER_SIZE);
              if (ret == -1) {
                std::cout << "Network Monitor: Write Error\n"
                  << strerror(errno) << std::endl;
              }
              if (max_fd < clients[numClients]) max_fd = clients[numClients];
              ++numClients;
            }
          } else {
            for (int i = 0; i < numClients; ++i) {
              if (FD_ISSET(clients[i], &read_fd_set)) {
                bzero(buffer, BUFFER_SIZE);
                ret = read(clients[i], buffer, BUFFER_SIZE); // Read data from clients[i]
                std::cout << buffer;
                if (ret == -1) {
                  std::cout << "Network Monitor: Read Error\n"
                    << strerror(errno) << std::endl;
                }
              }
            }
          }
        }
        sleep(1);
      }

      for (int i = 0; i < numClients; ++i) {
        std::cout << "Network Monitor: Requesting Client " << i+1 << " to quit." << std::endl;
        len = sprintf(buffer, "Quit") + 1;
        ret = write(clients[i], buffer, len);
        if (ret == -1) {
          std::cout << "Network Monitor: Write Error\n"
            << strerror(errno) << std::endl;
        }
        sleep(1);
        FD_CLR(clients[i], &active_fd_set);
        close(clients[i]);
      }

      // Wait for all responses to end
      while (wait(NULL) > 0);
      // Close master socket
      close(master_fd);
      // Remove socket file
      unlink(SOCKET_PATH);
    }
  }
}
