#include "../include/transmitter.hpp"
#include <iostream>
#include <unistd.h> // write

RSTransmit::RSTransmit(int fd) : m_fd(fd) {}

void RSTransmit::sendData(std::string data) {
  data += "\n";
  auto bytesWritten = write(m_fd, data.c_str(), data.length());
  if (bytesWritten < 0) {
    std::cerr << "Write error!\n";
  } else {
    std::cout << "Transmit: " << data << std::endl;
    std::cout << "Transmit Data Length: " << data.size() << std::endl;

  }
}
