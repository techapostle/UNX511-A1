#include <iostream>
#include <signal.h>

namespace nimutils {
  // static globals
  static bool terminate = false; // determines whether the process should terminate.

  // static function for signal handling
  static void signalHandler(int signal) {
    switch (signal) {
    case SIGINT:
      std::cout << "Received signal " << signal << ": SIGINT." << std::endl;
      terminate = true;
      break;
    default:
      std::cout << "Unhandled signal " << signal << ": UNKNOWN." << std::endl;
    }
  }
}
