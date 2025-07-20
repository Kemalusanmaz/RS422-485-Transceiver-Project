#include "../include/transmitter.hpp"
#include <iostream>
#include <unistd.h> // write

RSTransmit::RSTransmit(int fd) : m_fd(fd) {}

void RSTransmit::sendData(std::string data) {
  data += "\n"; // Append a newline character to the data.
  // The write() system call attempts to write data to a file descriptor.
  auto bytesWritten = write(m_fd, data.c_str(), data.length());
  if (bytesWritten < 0) { // Check if the write operation resulted in an error.
    std::cerr << "Write error!\n";
  }
  // else {
  // Print the transmitted data and its length to the console for
  // confirmation.
  // std::cout << "Transmit: " << data << std::endl;
  // std::cout << "Data Length: " << data.size() << std::endl;
  // }
}
