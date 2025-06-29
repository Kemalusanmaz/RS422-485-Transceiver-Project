#ifndef TRANSMITTER_HPP
#define TRANSMITTER_HPP
#include <string>

/**
 * @class RSTransmit
 * @brief Manages the transmission of data to a serial port.
 *
 * This class encapsulates the logic for writing data to a serial port.
 * It takes an existing file descriptor and provides a method to send
 * string-based data.
 */
class RSTransmit {
public:
  /**
   * @brief Constructs a new RSTransmit object.
   * @param fd The file descriptor of the already opened and configured
   *           serial port. This descriptor will be used for all write
   *           operations.
   */
  RSTransmit(int fd);

  /**
   * @brief Sends a string of data to the serial port.
   *
   * This method takes a std::string, appends a newline character ('\n') to
   * act as a message delimiter, and then writes the resulting data to the
   * serial port. A robust write loop should be implemented in the .cpp file
   * to ensure all data is sent.
   *
   * @param data The string data to be transmitted.
   * @note It is recommended that the implementation of this function appends a
   *       terminator like '\n' to facilitate message framing on the receiver's
   *       end.
   */
  void sendData(std::string data);

private:
  int m_fd; ///< The file descriptor for the serial port.
};

#endif // TRANSMITTER_HPP
