#ifndef MESSAGEHANDLER_HPP
#define MESSAGEHANDLER_HPP

#include "iMessageParser.hpp"
#include <string>

class MessageHandler {
public:
  IMessageParser *processMessage(const std::string &rawMessage);
};

#endif
