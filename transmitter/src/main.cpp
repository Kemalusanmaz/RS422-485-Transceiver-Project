#include "../../configuration/include/configuration.hpp"
#include "../include/transmitter.hpp"
#include <iostream>
#include <string>
#include <termios.h>

int main() {
  RSConfiguration config;
  config.initialize(config.getJsonData()["transmitterDeviceName"]);
  config.setRsConfig(config.getJsonData()["baudrate"]);
  RSTransmit transmitter(config.getFd());
  transmitter.sendData("start");
  int confOption;
  while (true) {
    std::cout << "---------RS Transmitter---------" << std::endl;
    std::cout << "0- Send Data\n"
                 "1- Exit\n"
              << std::endl;
    std::cout << "Enter a valid number: ";
    std::cin >> confOption;
    std::cout << std::endl;
    if (confOption == 0) {
      while (true) {
        std::string userData;
        std::cout << "Data: ";
        std::cin >> userData;
        transmitter.sendData(userData);
        continue;
      }
      continue;
    } else if (confOption == 1) {
      break;
    } else {
      std::cout << "Invalid number. Please Choose Proper Number" << std::endl;
      continue;
    }
  }
}