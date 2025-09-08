#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "../../common/include/common.hpp"
#include <filesystem>
#include <fstream>
#include <string>

class Logger {
public:
  // Logger();
  static void init(const char *executablePath);
  void openFile();
  void logTxt(std::string receiveData);
  void closeFile();

private:
  std::fstream m_file;
  static std::filesystem::path s_logDirectory;
  Common common;
};

#endif
