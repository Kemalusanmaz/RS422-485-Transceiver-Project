#ifndef SERIALPORTSETTINGS_HPP
#define SERIALPORTSETTINGS_HPP
#include <string>

class SerialPortSettings {
public:
  int getBaudrate() const { return baudrate; }
  void setBaudrate(int baudrate_) { baudrate = baudrate_; }

  int getDataBits() const { return dataBits; }
  void setDataBits(int dataBits_) { dataBits = dataBits_; }

  std::string getParity() const { return parity; }
  void setParity(const std::string &parity_) { parity = parity_; }

  int getStopBits() const { return stopBits; }
  void setStopBits(int stopBits_) { stopBits = stopBits_; }

  std::string getChannel() const { return channel; }
  void setChannel(const std::string &channel_) { channel = channel_; }

private:
  int baudrate;       //  baudrate
  int dataBits;       //  data bits (5, 6, 7, 8)
  std::string parity; //  parity ("None", "Even", "Odd")
  int stopBits;       // stop bits (1, 2)
  std::string channel;
};

#endif // SERIALPORTSETTINGS_HPP
