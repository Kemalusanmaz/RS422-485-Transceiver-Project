#include "../include/receiver.hpp"
#include <iostream>
#include <string>
#include <unistd.h> // read
#include <vector>

RSReceive::RSReceive(int fd) : m_fd(fd) { logger.openFile(); }

RSReceive::~RSReceive() { logger.closeFile(); }

void RSReceive::receiveData(size_t bufferSize) {

  // A temporary buffer to hold the data read from the port in a single call.
  // This buffer is created and destroyed each time the function is called.
  std::vector<char> buffer(bufferSize);

  // The read() system call attempts to read data from a file descriptor.
  // 1st arg (m_fd): The file descriptor for the serial port.
  // 2nd arg (buffer.data()): A pointer to the memory where read data will be
  // stored. 3rd arg (buffer.size()): The maximum number of bytes to read.
  ssize_t bytesRead = read(m_fd, buffer.data(), buffer.size());

  // A vector to store any complete messages found during this function call.
  std::vector<std::string> completedMessages;

  if (bytesRead > 0) { // Proceed only if data was actually read from the port.
    // Append the newly read data to the internal class buffer.
    m_internalBuffer.append(buffer.data(), bytesRead);

    // Search for the newline character ('\n') in the internal buffer.
    size_t pos;
    // The while loop continues as long as a newline is found.
    while ((pos = m_internalBuffer.find('\n')) != std::string::npos) {
      // When a newline is found, extract the message.
      // substr(0, pos) creates a new string from the beginning of the buffer
      // up to (but not including) the newline character.
      std::string message = m_internalBuffer.substr(0, pos);
      completedMessages.push_back(message);

      // Remove the extracted message and the newline character from the
      // buffer. This leaves the buffer ready for the next message, which might
      // already be partially present. pos + 1 includes the '\n' in the removal.
      m_internalBuffer.erase(0, pos + 1);
    }
  }
  // After processing, check if any complete messages were extracted.
  if (!completedMessages.empty()) {
    // If so, iterate through the vector and print each message.
    for (const auto &msg : completedMessages) {
      std::cout << common.getCurrentTime() << ": " << msg << std::endl;
      if (!msg.empty()) {
        IMessageParser *parsedResult = msgHndlr.processMessage(msg);
        if (parsedResult) {
          logger.logTxt(msg, parsedResult);
          delete parsedResult;
          parsedResult = nullptr;
        } else {
          logger.logTxt(msg);
        }
      }
    }
  }
}
