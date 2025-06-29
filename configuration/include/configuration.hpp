#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP

#include <fcntl.h>
#include <nlohmann/json.hpp>
#include <termios.h>
#include <unistd.h>

class RSConfiguration {
public:
  RSConfiguration();
  void initialize(const std::string &deviceStr);
  void setRsConfig(int baudrate);
  void terminate();

  int getFd() const { return fd; }

  nlohmann::json getJsonData() const { return jsonData; }

private:
  void jsonParser();
  int fd{};                // File descriptor for the RS device.
  nlohmann::json jsonData; // Parsed JSON configuration data.
  struct termios tty;

  void checkError(int ret, const char *msg);

  speed_t getBaudrateConstant(int baudrate);
};

#endif // CONFIGURATION_HPP
