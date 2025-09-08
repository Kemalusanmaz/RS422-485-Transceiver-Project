#include "../include/logger.hpp"
#include <cstdlib>
#include <filesystem>
#include <ios>
#include <iostream>
#include <ostream>

std::filesystem::path Logger::s_logDirectory;

void Logger::init(const char *executablePath) {
  std::filesystem::path exePath(executablePath);
  s_logDirectory = exePath.parent_path() / "log";
  std::filesystem::create_directory(s_logDirectory);
}

void Logger::openFile() {
  std::filesystem::path logFilePath = s_logDirectory / "log.txt";
  m_file.open(logFilePath, std::ios::out | std::ios::binary | std::ios::app);

  if (!m_file.is_open()) {
    std::cout<<"file can not be open!!!"<<std::endl;
  }
}

void Logger::logTxt(std::string receiveData) {

  if (!m_file.is_open()) {
    std::cout << "File cannot be open!!" << std::endl;
  } else {
    auto currentTime = common.getCurrentTime();
    m_file << currentTime << ": " << receiveData << std::endl;
    m_file.flush();
  }
}

void Logger::closeFile() { m_file.close(); }
