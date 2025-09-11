#include "../include/messageHandler.hpp"
#include "../include/mtmParser.hpp"
#include <cstdint>
#include <string>
#include <sys/types.h>

IMessageParser *MessageHandler::processMessage(const std::string &rawMessage) {
  std::string destAddrHex = rawMessage.substr(0, 2);
  uint8_t destAddrInt =
      static_cast<uint8_t>(std::stoul(destAddrHex, nullptr, 16));

  IMessageParser *parser = nullptr;

  switch (destAddrInt) {
  case 0x43:
    parser = new MtmParser();
    break;
  case 0x44:
    parser = new MtmParser();
    break;
  default:
    return nullptr;
  }

  if (parser != nullptr) {
    parser->messageParser(rawMessage);
    parser->display();
  }

  return parser;
}
