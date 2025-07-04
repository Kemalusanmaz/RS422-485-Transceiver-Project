#include "../../configuration/include/configuration.hpp"
#include "../include/receiver.hpp"
#include <iostream>
#include <thread>

int main() {
  RSConfiguration config;
  tcflush(config.getFd(), TCIOFLUSH);
  config.terminate();
  config.initialize(config.getJsonData()["receiverDeviceName"]);
  config.setRsConfig(config.getJsonData()["baudrate"]);
  RSReceive receiver(config.getFd());
  int confOption = -1;

  while (true) {
    std::cout << "---------RS Receiver---------" << std::endl;
    std::cout << "0- Receive Data\n"
                 "1- Exit\n"
              << std::endl;
    std::cout << "Enter a valid number: ";
    std::cin >> confOption;
    std::cout << std::endl;
    if (confOption == 0) {
      while (true) {

        receiver.receiveData(1024);
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
      }
    }

    else if (confOption == 1) {
      break;
    } else {
      std::cout << "Invalid number. Please Choose Proper Number" << std::endl;
      continue;
    }
  }
}