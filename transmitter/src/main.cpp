#include "../../configuration/include/configuration.hpp"
#include "../../configuration/include/serialPortSettings.hpp"
#include "../include/transmitter.hpp"
#include <chrono>
#include <iostream>
#include <string>
#include <termios.h>
#include <thread>

int main() {
  RSConfiguration config;
  SerialPortSettings settings;

  settings.setBaudrate(config.getJsonData()["baudrate"]);
  settings.setDataBits(config.getJsonData()["dataBits"]);
  settings.setParity(config.getJsonData()["parity"]);
  settings.setStopBits(config.getJsonData()["stopBits"]);

  config.initialize(config.getJsonData()["transmitterDeviceName"]);
  config.setRsConfig(settings);
  RSTransmit transmitter(config.getFd());
  transmitter.sendData("\b\b");
  int confOption;
  while (true) {
    std::cout << "---------RS Transmitter---------" << std::endl;
    std::cout << "0- Send Data\n"
                 "1- Send Data Cyclically\n"
                 "2- Exit\n"
              << std::endl;
    std::cout << "Enter a valid number: ";
    std::cin >> confOption;
    std::cout << std::endl;
    std::string userData;
    std::cout << "Data: ";
    std::cin >> userData;

    if (confOption == 0) {
      transmitter.setCycleFlag(false);
      while (true) {
        transmitter.sendData(userData);
        continue;
      }
    } else if (confOption == 1) {
      transmitter.setCycleFlag(true);
      while (transmitter.getCycleFlag() == 1) {
        transmitter.sendData(userData);
        std::this_thread::sleep_for(std::chrono::microseconds(1000));
      }

    } else if (confOption == 2) {
      break;
    } else {
      std::cout << "Invalid number. Please Choose Proper Number" << std::endl;
      continue;
    }
  }
}