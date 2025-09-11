#ifndef IMESSAGEPARSER_HPP
#define IMESSAGEPARSER_HPP

#include <string>
class IMessageParser {
public:
  virtual ~IMessageParser() = default;
  virtual void messageParser(const std::string &rawMessage) = 0;
  virtual void display() = 0;
};

#endif // IMESSAGEPARSER_HPP
