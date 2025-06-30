#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <cstddef>
#include <string>

/**
 * @class RSReceive
 * @brief Manages the reception and buffering of data from a serial port.
 *
 * This class encapsulates the low-level logic of reading from a serial port's
 * file descriptor. It uses an internal string buffer to accumulate incoming
 * data, which is essential for correctly handling messages that may arrive in
 * multiple chunks (partial reads).
 */
class RSReceive {
public:
  /**
   * @brief Constructs a new RSReceive object.
   * @param fd The file descriptor of the already opened and configured
   *           serial port.
   */
  RSReceive(int fd);

  /**
   * @brief Reads available data from the port into the internal buffer.
   *
   * This function should be implemented to perform a single read operation from
   * the serial port and append the received bytes into the m_internalBuffer.
   * It serves as the low-level mechanism for a higher-level message parsing
   * function.
   *
   * @param bufferSize The maximum number of bytes to read in a single `read()`
   * call.
   *
   * @note To get complete, framed messages, you should create a new function
   *       (e.g., `std::vector<std::string> receiveMessages()`) that calls this
   *       logic internally and then parses the `m_internalBuffer` for a
   *       delimiter like '\n', returning a vector of complete messages.
   */
  void receiveData(size_t bufferSize);

private:
  int m_fd;                     ///< The file descriptor for the serial port.
  std::string m_internalBuffer; ///< Buffer to accumulate incoming data and
                                ///< handle fragmented messages.
};

#endif // RECEIVER_HPP
