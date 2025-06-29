#ifndef TRANSMITTER_HPP
#define TRANSMITTER_HPP
#include <string>

class RSTransmit {
public:
  RSTransmit(int fd);

  void sendData(std::string data);

private:
  int m_fd;
};

#endif // TRANSMITTER_HPP
