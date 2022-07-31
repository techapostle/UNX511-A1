#ifndef UTILS_CPP
#define UTILS_CPP

#include <iostream>
#include <signal.h>

namespace nimutils {
  // Define some important constants
  #define BUFFER_SIZE 200 // Length of buffer
  #define MAX_BUFFER 64
  #define MAX_CLIENTS 5 // Maximum number of clients
  #define SOCKET_PATH "/tmp/netmon" // Path to socket file

  // static globals
  static bool terminate_NM = false; // determines whether the process should terminate.

  // static function for signal handling
  static void signalHandler(int signal) {
    switch (signal) {
    case SIGINT:
      std::cout << "Received signal " << signal << ": SIGINT." << std::endl;
      terminate_NM = true;
      break;
    default:
      std::cout << "Unhandled signal " << signal << ": UNKNOWN." << std::endl;
    }
  }
}

#endif
