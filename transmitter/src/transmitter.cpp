#include "../include/transmitter.hpp"
#include <cstdint> // uint8_t için
#include <iomanip> // std::hex, std::setw, std::setfill için
#include <iostream>
#include <termios.h>
#include <unistd.h> // write

RSTransmit::RSTransmit(int fd) : m_fd(fd) {}

void RSTransmit::sendDataHex(const std::vector<uint8_t> &hexData) {
  // write() fonksiyonu `const char*` bekler, bu yüzden `reinterpret_cast`
  // kullanırız. `uint8_t*` işaretçisini `char*` işaretçisine dönüştürür.
  auto bytesWritten = write(
      m_fd, reinterpret_cast<const char *>(hexData.data()), hexData.size());

  std::cout << "Transmit (Hex): ";
  for (uint8_t byte : hexData) {
    std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)byte
              << " ";
  }
  std::cout << std::dec << std::endl; // Tekrar decimal moda geçiş yap
  std::cout << "Data Length: " << hexData.size() << std::endl;

  if (bytesWritten < 0) {
    std::cerr << " HEX Write error!\n";
  } else if (bytesWritten != hexData.size()) {
    std::cerr << "Warning: Only " << bytesWritten << " bytes written, expected "
              << hexData.size() << std::endl;
  }

  tcdrain(m_fd); // Tüm verinin fiziksel olarak gönderildiğinden emin ol
}

void RSTransmit::sendData(std::string data) {
  data += "\n"; // Append a newline character to the data.
  // The write() system call attempts to write data to a file descriptor.
  auto bytesWritten = write(m_fd, data.c_str(), data.length());
  // Print the transmitted data and its length to the console for
  // confirmation.
  // Ensure all written data has been transmitted
  tcdrain(m_fd); // Wait until all output has been transmitted
  std::cout << "Transmit (ASCII): " << data << std::endl;
  std::cout << "Data Length: " << data.size() << std::endl;
  if (bytesWritten < 0) { // Check if the write operation resulted in an error.
    std::cerr << "ASCII Write error!\n";
  }
}
