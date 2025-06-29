#ifndef RECEIVER_HPP
#define RECEIVER_HPP

#include <cstddef>
#include <string>
class RSReceive {
public:
  RSReceive(int fd);

  void receiveData(size_t bufferSize);

  private:
  int m_fd;
  std::string m_internalBuffer;
};

#endif // RECEIVER_HPP
