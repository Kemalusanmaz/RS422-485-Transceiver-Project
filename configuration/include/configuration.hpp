#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <fcntl.h>
#include <nlohmann/json.hpp>
#include <termios.h>
#include <unistd.h>

/**
 * @class RSConfiguration
 * @brief Manages the configuration and lifecycle of a serial port connection.
 *
 * This class encapsulates all tasks related to setting up and tearing down a
 * serial port (RS232/422/485). It reads parameters from a JSON file,
 * opens the specified device, configures it using termios, and provides
 * methods to safely close it.
 */
class RSConfiguration {
public:
  /**
   * @brief Constructs a new RSConfiguration object.
   *
   * The constructor automatically calls jsonParser() to load the
   * configuration from the default JSON file path upon object creation.
   */
  RSConfiguration();

  /**
   * @brief Initializes and opens the serial port device.
   * @param deviceStr The path to the serial device file (e.g., "/dev/ttyUSB0").
   * @note This function must be called before any other operations like
   *       setRsConfig() or any read/write calls can be made.
   */
  void initialize(const std::string &deviceStr);

  /**
   * @brief Configures the serial port with specified settings using termios.
   * @param baudrate The desired baud rate as an integer (e.g., 9600, 115200).
   *                 This value is internally converted to a termios speed
   * constant.
   * @note This function sets the port to 8N1 (8 data bits, no parity, 1 stop
   * bit), disables flow control, and configures the port for raw data mode.
   */
  void setRsConfig(int baudrate);

  /**
   * @brief Closes the serial port file descriptor.
   *
   * Safely terminates the connection to the serial device. It's good practice
   * to call this function when the port is no longer needed.
   */
  void terminate();

  /**
   * @brief Gets the file descriptor of the opened serial port.
   * @return The integer file descriptor. Returns a value less than 0 if the
   *         port is not open or an error occurred.
   */
  int getFd() const { return fd; }

  nlohmann::json getJsonData() const { return jsonData; }

private:
  /**
   * @brief Gets the parsed JSON configuration data.
   * @return A const nlohmann::json object containing the configuration
   *         loaded from the file.
   */
  void jsonParser();
  /**
   * @brief Checks the return value of a function for errors and exits on
   * failure.
   * @param ret The return value of the function to check. Typically, 0 means
   * success.
   * @param msg A descriptive error message to print if an error is detected.
   * @private
   */
  void checkError(int ret, const char *msg);
  /**
   * @brief Converts an integer baud rate to a termios speed_t constant.
   * @param baudrate The integer representation of the baud rate (e.g., 9600).
   * @return The corresponding termios constant (e.g., B9600). Returns B0 if
   *         the baud rate is unsupported.
   * @private
   */
  speed_t getBaudrateConstant(int baudrate);
  
  int fd{};                ///< File descriptor for the opened serial device.
  nlohmann::json jsonData; // Parsed JSON configuration data.
  struct termios tty;      ///< termios struct to hold serial port settings.
};

#endif // CONFIGURATION_HPP
