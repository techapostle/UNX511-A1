/*  ╭──────────────────────────────────╮
 *  │ Name: Kian Dadkhah Shokrollahi   │
 *  │ SID: 119369205                   │
 *  │ UNX511 Assignment 1              │
 *  ╰──────────────────────────────────╯
 */

/* --- Network Interface Monitor: Utilities ---
 * This unit contains the utilities shared across the Network Interface Monitor
 * units.
 */

#ifndef UTILS_CPP
#define UTILS_CPP

#include <cstring>
#include <fstream>
#include <iostream>
#include <net/if.h>
#include <signal.h>
#include <sys/ioctl.h>

// Define some important constants
#define BUFFER_SIZE 200 // Length of buffer
#define MAX_BUFFER 64
#define MAX_CLIENTS 5             // Maximum number of clients
#define SOCKET_PATH "/tmp/netmon" // Path to socket file

namespace nimutils {

// static globals
static bool terminate_NM =
    false; // determines whether the process should terminate.
static int skfd = -1;

// function for signal handling
void signalHandler(int signal) {
  switch (signal) {
  case SIGINT:
    std::cout << "Received signal " << signal << ": SIGINT." << std::endl;
    terminate_NM = true;
    break;
  default:
    std::cout << "Unhandled signal " << signal << ": UNKNOWN." << std::endl;
  }
}

int set_if_flags(char *ifname, short flags) {
  struct ifreq ifr;
  int res = 0;
  ifr.ifr_flags = flags;
  strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
  skfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (skfd < 0) {
    std::cout << "Socket Error: " << strerror(errno);
    res = 1;
    return res;
  }
  res = ioctl(skfd, SIOCSIFFLAGS, &ifr);
  if (res < 0) {
    std::cout << "Interface " << ifname
              << " Error: SIOCSIFFLAGS failed: " << strerror(errno);
  } else {
    std::cout << "Interface " << ifname << ": Link Up!" << std::endl;
  }
  return res;
}

int set_if_up(char *ifname, short flags) {
  return set_if_flags(ifname, flags | IFF_UP);
}

void getInterfaceStat(char *interface, std::string &data) {
  std::string operstate;
  char stat_path[MAX_BUFFER * 2];
  int carrier_up_count = 0;
  int tx_bytes = 0, tx_packets = 0, tx_dropped = 0, tx_errors = 0;
  int rx_bytes = 0, rx_packets = 0, rx_dropped = 0, rx_errors = 0;
  std::ifstream infile;

  sprintf(stat_path, "/sys/class/net/%s/operstate", interface);
  infile.open(stat_path);
  if (infile.is_open()) {
    infile >> operstate;
    infile.close();
  }
  sprintf(stat_path, "/sys/class/net/%s/carrier_up_count", interface);
  infile.open(stat_path);
  if (infile.is_open()) {
    infile >> carrier_up_count;
    infile.close();
  }
  sprintf(stat_path, "/sys/class/net/%s/statistics/tx_bytes", interface);
  infile.open(stat_path);
  if (infile.is_open()) {
    infile >> tx_bytes;
    infile.close();
  }
  sprintf(stat_path, "/sys/class/net/%s/statistics/rx_bytes", interface);
  infile.open(stat_path);
  if (infile.is_open()) {
    infile >> rx_bytes;
    infile.close();
  }
  sprintf(stat_path, "/sys/class/net/%s/statistics/rx_dropped", interface);
  infile.open(stat_path);
  if (infile.is_open()) {
    infile >> rx_dropped;
    infile.close();
  }
  sprintf(stat_path, "/sys/class/net/%s/statistics/rx_errors", interface);
  infile.open(stat_path);
  if (infile.is_open()) {
    infile >> rx_errors;
    infile.close();
  }
  sprintf(stat_path, "/sys/class/net/%s/statistics/tx_packets", interface);
  infile.open(stat_path);
  if (infile.is_open()) {
    infile >> tx_packets;
    infile.close();
  }
  sprintf(stat_path, "/sys/class/net/%s/statistics/tx_dropped", interface);
  infile.open(stat_path);
  if (infile.is_open()) {
    infile >> tx_dropped;
    infile.close();
  }
  sprintf(stat_path, "/sys/class/net/%s/statistics/tx_errors", interface);
  infile.open(stat_path);
  if (infile.is_open()) {
    infile >> tx_errors;
    infile.close();
  }
  sprintf(stat_path, "/sys/class/net/%s/statistics/rx_packets", interface);
  infile.open(stat_path);
  if (infile.is_open()) {
    infile >> rx_packets;
    infile.close();
  }

  if (operstate == "down") {
    set_if_up(interface, 1);
  }

  std::string tmp_interface = std::string(interface);
  std::string tmp_operstate = std::string(operstate);

  data = "Interface: " + tmp_interface + " state: " + tmp_operstate +
         " up_count: " + std::to_string(carrier_up_count) + "\n" +
         " rx_bytes: " + std::to_string(rx_bytes) +
         " rx_dropped: " + std::to_string(rx_dropped) +
         " rx_errors: " + std::to_string(rx_errors) +
         " rx_packets: " + std::to_string(rx_packets) +
         " \n tx_bytes: " + std::to_string(tx_bytes) +
         " tx_dropped: " + std::to_string(tx_dropped) +
         " tx_errors: " + std::to_string(tx_errors) + " \n\n";
}
} // namespace nimutils

#endif
