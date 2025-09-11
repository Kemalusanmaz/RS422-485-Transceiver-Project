#include "../include/configuration.hpp"
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

std::filesystem::path RSConfiguration::s_configPath;

// Initializes member variables and automatically calls jsonParser() to load the
// configuration upon object creation.
RSConfiguration::RSConfiguration() { jsonParser(); }

void RSConfiguration::init(const char *executablePath) {
  if (executablePath == nullptr) {
    return;
  }

  std::filesystem::path exeDir = std::filesystem::path(executablePath).parent_path();

  s_configPath = exeDir / "rsConfig.json";
}

// Parses JSON config file and loads it into jsonData
void RSConfiguration::jsonParser() {
  
  std::ifstream jsonFile(
      s_configPath); // Create an input file stream to read the file.
  if (!jsonFile.is_open()) {
    std::cerr << "JSON file could not be opened!"
              << std::endl; // error if the file cannot be opened
  }
  // else {
  //   std::cout << "JSON file is opened successfuly" << std::endl;
  // }

  // Use the nlohmann::json library's stream operator (>>) to parse
  // the entire file content directly into the jsonData object.
  jsonFile >> jsonData;
}

// Checks the return value of a function for errors.
void RSConfiguration::checkError(int ret, const char *msg) {
  if (ret != 0) {
    // strerror() converts an error number (errno) into a human-readable string
    std::cerr << msg << " Error Code: " << strerror(-ret) << std::endl;
    // Clean up by closing the file descriptor before exiting.
    close(fd);
    exit(EXIT_FAILURE);
  }
}

// Converts an integer baud rate value (e.g., 9600) into the corresponding
// speed_t constant required by the termios library (e.g., B9600).
speed_t RSConfiguration::getBaudrateConstant(int baudrate) {
  switch (baudrate) {
  case 9600:
    return B9600;
  case 19200:
    return B19200;
  case 38400:
    return B38400;
  case 57600:
    return B57600;
  case 115200:
    return B115200;
  case 230400:
    return B230400;
  case 460800:
    return B460800;
  case 500000:
    return B500000;
  case 576000:
    return B576000;
  case 921600:
    return B921600;
  case 1000000:
    return B1000000;
  case 1152000:
    return B1152000;
  case 1500000:
    return B1500000;
  case 2000000:
    return B2000000;
  default:
    return B0; // B0 is a special value, often used to signal an unsupported
               // baud rate or to hang up.
  }
}

// Initializes the serial port by opening the device file.
void RSConfiguration::initialize(const std::string &deviceStr) {

  // The open() system call requires a C-style string (const char*).
  const char *device = deviceStr.c_str();

  // open() returns a file descriptor (a small integer) for the device.
  // Flags used:
  // O_RDWR:   Open for both reading and writing.
  // O_NOCTTY: The device will not become the process's controlling terminal.
  //           This is crucial for serial port programming to prevent unwanted
  //           signals.
  // O_SYNC:   Writes are synchronized, ensuring they complete before returning.
  fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK | O_SYNC);

  if (fd < 0) { // if there is an error, return -1
    // perror() prints the given string, followed by a colon, a space,
    // and a human-readable message for the current system error code (errno).
    perror("open");
  } else {
    std::cout << "RS Channel File is opened successfully!" << std::endl;
  }
}

// Configures the serial port attributes using the termios library.
void RSConfiguration::setRsConfig(SerialPortSettings settings) {
  auto convertBaudrate = getBaudrateConstant(settings.getBaudrate());
  memset(&tty, 0, sizeof(tty));

  // tcgetattr() gets the current attributes of the serial port associated
  // with 'fd' and stores them in the 'tty' struct. These attributes are
  // acquired to modify the existing settings rather than starting from scratch.
  int ret = tcgetattr(fd, &tty);
  checkError(ret, "Error from tcgetattr");

  // Set input and output baud rates.
  cfsetospeed(&tty, convertBaudrate);
  cfsetispeed(&tty, convertBaudrate);

  // --- c_cflag: Control Flags (Hardware-level settings) ---
  // These flags define the physical properties of the line, like data bits,
  // parity, etc.

  // CSIZE clear the character size mask (& ~CSIZE)
  // CS8  set the data bits to 8 (| CS8).
  // Result: Every data package has 8 bit length
  tty.c_cflag &= ~CSIZE; // Önce mevcut boyutu temizle
  switch (settings.getDataBits()) {
  case 5:
    tty.c_cflag |= CS5;
    break;
  case 6:
    tty.c_cflag |= CS6;
    break;
  case 7:
    tty.c_cflag |= CS7;
    break;
  case 8:
  default:
    tty.c_cflag |= CS8;
    break;
  }

  // PARENB NO parity (&= ~PARENB)
  // PARENB Enable parity checking (|= PARENB)
  // PARODD odd parity (|= PARODD)
  // PARODD even parity (&= ~PARODD)
  tty.c_cflag |= PARENB | PARODD;
  if (settings.getParity() == "Odd") {
    tty.c_cflag |= PARENB;
    tty.c_cflag |= PARODD;
  } else if (settings.getParity() == "Even") {
    tty.c_cflag |= PARENB;
    tty.c_cflag &= ~PARODD;
  } else {
    tty.c_cflag &= ~PARENB;
  }

  // Defines how many stop bits after every raw data package.
  // CSTOPB Set to use 2 stop bits (|= CSTOPB)
  // CSTOPB Set to use 1 stop bits (&= ~CSTOPB)
  if (settings.getStopBits() == 2) {
    tty.c_cflag |= CSTOPB; // 2 stop biti
  } else {
    tty.c_cflag &= ~CSTOPB; // 1 stop biti (varsayılan)
  }

  // ıt makes that disable Hardware Flow Control. RTS (Request to Send)
  // and CTS (Clear to Send) pins cannot be used. when receiver buffer is
  // fulled,these pins provides to send stop signal to the transmitter.
  // CRTSCTS Disable hardware flow control (&= ~CRTSCTS)
  // CRTSCTS Enable hardware flow control (|= CRTSCTS)
  tty.c_cflag &= ~CRTSCTS;

  // CREAD: Enable the receiver. A data can not received without this flas is
  // opened. CLOCAL: Ignore modem control lines ( DCD, DSR, RI, carrier
  // detect). This flag shows device is coonect another device not a modem. This
  // ensures the port can be opened without a modem signaling it's ready.
  tty.c_cflag |= CREAD | CLOCAL;

  // --- c_lflag: Local Flags (Terminal-like behavior) ---
  // These flags control high-level processing (e.g. echo, signal processing).
  // It determines that how much received data will be procesed by tge driver.
  // Disabling them all puts the terminal into "raw mode," which is ideal for
  // machine-to-machine communication.

  // ICANON: Disable canonical mode. This makes input available byte-by-byte
  //         instead of line-by-line by driver. the program does not give until
  //         Enter\n() is pressed. Additionally, It also process editing chars
  //         such as Backspace.
  // ECHO: Disable echoing of input characters back to the sender.
  // ECHOE:  Disable visual erasure of characters on backspace.
  // ISIG:   Disable signal-generating characters (like Ctrl-C (SIGINT), Ctrl-Z
  // (SIGTSTP).
  tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

  // --- c_iflag: Input Flags (Input data processing) ---
  //    Flags related to processing of receiving data. Controls whether
  //    characters are converted during data flow. Again, best to turn most of
  //    them off for "raw mode"

  // In this system, when the receiver's buffer is full, a special STOP
  // character (Ctrl-S) is sent to the sender, and when it is empty, a START
  // character (Ctrl-Q) is sent. XON/XOFF Disable software flow control
  // (XON/XOFF), which can interfere with binary data.
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);

  // --- c_oflag: Output Flags (Output data processing) ---
  //   Flags related to the processing of transmitting data.

  //   OPOST  Disable all output processing (OPOST). This prevents conversions
  //   like mapping newline ('\n') to carriage return + newline ("\r\n").
  tty.c_oflag &= ~OPOST;

  // --- c_cc: Control Characters (Timeout and blocking settings) ---
  // VMIN: Minimum number of characters to read before read() returns.
  // VTIME: Timeout in tenths of a second.

  // This combination (VMIN > 0, VTIME > 0) means:
  // read() will wait indefinitely for the first character (VMIN=1).
  // After the first character is received, it will wait up to VTIME tenths of
  // a second for subsequent characters before returning.
  tty.c_cc[VMIN] = 1;
  tty.c_cc[VTIME] = 1; // (means 0.1 second)

  // tcsetattr() applies the new settings to the serial port.
  // TCSANOW: The change occurs immediately.
  ret = tcsetattr(fd, TCSANOW, &tty);
  if (ret == 0) {
    std::cout << "RS Channel is set successfully!" << std::endl;
  } else {
    checkError(ret, "Error from tcsetattr");
  }
}

void RSConfiguration::getRsConfig(SerialPortSettings settings) {
  
  std::cout << settings.getChannel() <<" Channel is set successfully!" << std::endl;
  std::cout << "Baudrate: " << settings.getBaudrate() << std::endl;
  std::cout << "Data Bits: " << settings.getDataBits() << std::endl;
  std::cout << "Stop Bits: " << settings.getStopBits() << std::endl;
  std::cout << "Parity: " << settings.getParity() << std::endl;
}

// This function should be called to properly release the serial port resource
// when it is no longer needed.
void RSConfiguration::terminate() {
  close(fd);
  std::cout << "RS Channel File is closed successfully!" << std::endl;
}
