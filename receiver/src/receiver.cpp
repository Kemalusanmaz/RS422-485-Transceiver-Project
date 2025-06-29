#include <iostream>
#include <string>
#include <unistd.h> // read
#include <vector>

#include "../include/receiver.hpp"
RSReceive::RSReceive(int fd) : m_fd(fd) {}

void RSReceive::receiveData(size_t bufferSize) {
  std::vector<char> buffer(bufferSize);

  ssize_t bytesRead = read(m_fd, buffer.data(), sizeof(buffer));
  std::vector<std::string> completed_messages;

  if (bytesRead > 0) {
    // 3. Okunan yeni veriyi sınıfın dahili tamponuna ekle
    m_internalBuffer.append(buffer.data(), bytesRead);

    // 4. Dahili tamponda '\n' karakteri ara
    size_t pos;
    while ((pos = m_internalBuffer.find('\n')) != std::string::npos) {
      // 5. Bir '\n' bulundu! Mesajı ayır.
      std::string message = m_internalBuffer.substr(0, pos);
      completed_messages.push_back(message);

      // 6. Ayırdığın mesajı ve '\n' karakterini dahili tampondan sil.
      // Geriye bir sonraki mesajın başlangıcı (veya boşluk) kalır.
      m_internalBuffer.erase(0, pos + 1);
    }
  }
  if (!completed_messages.empty()) {
    for (const auto &msg : completed_messages) {
      std::cout << "Receive: " << msg << std::endl;
    }
  }
}
