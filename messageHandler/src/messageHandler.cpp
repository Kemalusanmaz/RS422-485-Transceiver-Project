#include "../include/messageHandler.hpp"
#include <bitset>
#include <cstddef>
#include <string>

void MessageHandler::messageParser(const std::string &rawHex) {
  if (rawHex.length() < 10 || rawHex.length() % 2 != 0) {
    return;
  }

  m_destinationAddress = rawHex.substr(0, 2);
  m_sourceAddress = rawHex.substr(2, 2);
  m_messageControl = rawHex.substr(4, 2);
  m_messageCrc = rawHex.substr(rawHex.length() - 4, 4);

  size_t dataLength = rawHex.length() - 10;
  if (dataLength > 0) {
    m_messageData = rawHex.substr(6, dataLength);
  } else {
    m_messageData = "";
  }
}

void MessageHandler::parseMessageControlByte() {
  uint8_t controlByte =
      static_cast<uint8_t>(std::stoul(m_messageControl, nullptr, 16));

  m_pollBit = (controlByte & 0x80) != 0; // 0x80 mask: 1000 0000
  m_bBit = (controlByte & 0x40) != 0; // 0x40 mask: 0100 0000
  m_aBit = (controlByte & 0x20) != 0; // 0x20 mask: 0010 0000
  
  uint8_t commandCode = (controlByte & 0x1F); // 0x1F mask: 0001 1111
  std::bitset<5> bits(commandCode);
  m_commandCode = bits.to_string();
}
