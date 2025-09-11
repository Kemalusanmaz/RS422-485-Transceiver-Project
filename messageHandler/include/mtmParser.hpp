#ifndef MTMPARSER_HPP
#define MTMPARSER_HPP

#include "iMessageParser.hpp"
#include <string>

class MtmParser : public IMessageParser {
public:
  void messageParser(const std::string &rawHex) override;

  void display() override;

  std::string getDestinationAddress() const { return m_destinationAddress; }

  std::string getSourceAddress() const { return m_sourceAddress; }

  std::string getMessageControl() const { return m_messageControl; }

  std::string getMessageData() const { return m_messageData; }

  std::string getMessageCrc() const { return m_messageCrc; }

  uint8_t getPollBit() const { return m_pollBit; }

  uint8_t getBBit() const { return m_bBit; }

  uint8_t getABit() const { return m_aBit; }

  std::string getCommandCode() const { return m_commandCode; }

private:
  void parseMessageControlByte();

  std::string m_destinationAddress; // identifies the receiver of the message 1
                                    // (0x01)...127 (0x7f)
  std::string m_sourceAddress;      // identifies the initiator of the message 1
                                    // (0x01)...127 (0x7f)
  std::string m_messageControl;
  std::string m_messageData;
  std::string m_messageCrc;

  uint8_t m_pollBit;
  uint8_t m_bBit;
  uint8_t m_aBit;
  std::string m_commandCode;
};

#endif // MTMPARSER_HPP
