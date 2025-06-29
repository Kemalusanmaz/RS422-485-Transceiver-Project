#include "../include/configuration.hpp"
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/types.h>
#include <unistd.h>

// Parses JSON config file and loads it into jsonData
void RSConfiguration::jsonParser() {
  std::string jsonPath = "/home/trick/kemal/RS422_485_Project/rsConfig.json";
  std::ifstream jsonFile(jsonPath);
  if (!jsonFile.is_open()) {
    std::cerr << "JSON file could not be opened!"
              << std::endl; // error if the file cannot be opened
  } else {
    std::cout << "JSON file is opened successfuly" << std::endl;
  }
  jsonFile >> jsonData;
}

void RSConfiguration::checkError(int ret, const char *msg) {
  if (ret != 0) {
    std::cerr << msg << " Error Code: " << strerror(-ret) << std::endl;
    close(fd);
    exit(EXIT_FAILURE);
  }
}

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
    return B0; // Geçersiz veya desteklenmeyen baud rate
  }
}

RSConfiguration::RSConfiguration() : fd(), jsonData(), tty() { jsonParser(); }

void RSConfiguration::initialize(const std::string &deviceStr) {
  const char *device = deviceStr.c_str();
  fd = open(
      device,
      O_RDWR | // open can channel device file with both read and write mode
          O_NOCTTY | O_SYNC);

  if (fd < 0) { // if there is an error, return -1
    perror("open");
  } else {
    std::cout << "RS Channel File is opened successfully!" << std::endl;
  }
}

void RSConfiguration::setRsConfig(int baudrate) {
  auto convertBaudrate = getBaudrateConstant(baudrate);
  memset(&tty, 0, sizeof tty);
  int ret = tcgetattr(fd, &tty);
  checkError(ret, "Error from tcgetattr");
  // Baud rate ayarı - 115200
  cfsetospeed(&tty, convertBaudrate);
  cfsetispeed(&tty, convertBaudrate);

  //   Control Flags (Kontrol Bayrakları) - Donanım seviyesi ayarları (baud
  //   rate, bit sayısı, parity vb.).
  //   iki cihaz arasındaki fiziksel elektrik sinyallerinin nasıl
  //   yorumlanacağını belirler.

  // CSIZE maskesiyle mevcut karakter boyutu ayarını temizler (&
  //   ~CSIZE) ve ardından CS8 ile yeniden ayarlar (| CS8). Sonuç: Her bir veri
  //   paketi 8 bit uzunluğunda olacak.
  tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;

  //  (Parity Enable) bayrağını temizler. Bu, Parity biti kontrolünü devre dışı
  //  bırakır. Bu en yaygın ayardır ("No Parity"). Parity'i etkinleştirmek için
  //  tty.c_cflag |= PARENB; yapabilirsiniz. Etkinleştirdikten sonra tek mi çift
  //  mi olacağını PARODD bayrağı ile belirlersiniz (|= PARODD tek parity, &=
  //  ~PARODD çift parity).
  tty.c_cflag |= PARENB | PARODD;

  //   CSTOPB (2 Stop Bits) bayrağını temizler. Bu, her veri paketinin sonunda
  //   sadece 1 Stop biti olacağını belirtir. 2 stop biti kullanmak için
  //   tty.c_cflag |= CSTOPB; yaparsınız.
  tty.c_cflag |= CSTOPB;

  //   CRTSCTS (RTS/CTS Hardware Flow Control) bayrağını temizler. Bu, donanım
  //   tabanlı akış kontrolünü devre dışı bırakır. Yani, RTS (Request to Send)
  //   ve CTS (Clear to Send) pinleri kullanılmaz. Donanım akış kontrolünü açmak
  //   için tty.c_cflag |= CRTSCTS; yaparsınız. Bu, alıcının tamponu dolduğunda
  //   göndericiye "dur" sinyali göndermesini sağlar.
  tty.c_cflag &= ~CRTSCTS;

  //   CREAD: Porttan okumayı etkinleştirir. Bu olmadan veri alamazsınız.
  // CLOCAL: Modem kontrol hatlarını (DCD, DSR, RI gibi) görmezden gel. Bu,
  // cihazınızın bir modeme değil, doğrudan başka bir cihaza bağlı olduğunu
  // varsayar. Bu bayrak ayarlanmazsa, port ancak bir "carrier detect" sinyali
  // aldığında açılır.
  tty.c_cflag |= CREAD | CLOCAL; // Okuma açık, yerel bağlantı

  //    Local Flags (Yerel Bayraklar) - Yüksek seviyeli, terminal benzeri
  //    davranışlar (echo, sinyal işleme vb.). elen verinin sürücü tarafından ne
  //    kadar "işleneceğini" belirler. Siz tümünü devre dışı bırakarak "raw
  //    mode" elde ediyorsunuz. Bu, veriyi geldiği gibi, hiçbir
  //    değişikliğe uğramadan almak için en doğru yoldur.

  // ICANON: Canonical Mode'u (Kurallı Mod) kapatır. Canonical mod açıkken,
  // sürücü veriyi satır satır biriktirir ve siz Enter'a (\n) basana kadar
  // programınıza vermez. Ayrıca Backspace gibi düzenleme karakterlerini de
  // işler. Kapatarak, veriyi byte byte, geldiği anda almanızı sağlarsınız.
  // ECHO: Gelen karakterlerin otomatik olarak porta geri gönderilmesini
  //   (yankılanmasını) engeller.
  // ECHOE: Silme karakteri geldiğinde imlecin geri gidip karakteri silmesini
  // engeller.
  // ISIG: Ctrl-C (SIGINT) veya Ctrl-Z (SIGTSTP) gibi sinyal üreten
  // karakterlerin işlenmesini engeller.
  tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
  //    Input Flags (Giriş Bayrakları) - Gelen verinin işlenmesiyle ilgili
  //    bayraklar.  veri akışı sırasında karakterlerin dönüştürülüp
  //    dönüştürülmeyeceğini kontrol eder. Yine, "raw mode" için çoğunu kapatmak
  //    en iyisidir.

  //   Yazılım tabanlı akış kontrolünü (XON/XOFF) devre dışı bırakır. Bu
  //   sistemde, alıcının tamponu dolduğunda göndericiye özel bir STOP karakteri
  //   (Ctrl-S) gönderilir, boşaldığında ise START karakteri (Ctrl-Q)
  //   gönderilir. Bunu kapatmak genellikle daha iyidir.
  tty.c_iflag &= ~(IXON | IXOFF | IXANY);

  //   Output Flags (Çıkış Bayrakları) - Giden verinin işlenmesiyle ilgili
  //   bayraklar.

  //   Çıktı işlemesini (Output Processing) devre dışı bırakır. Bu açıkken,
  //   sürücü \n (newline) karakterini \r\n (carriage return + newline) gibi
  //   karakter dizilerine çevirebilir. Bunu kapatarak, gönderdiğiniz byte
  //   dizisinin hiçbir değişikliğe uğramadan gönderilmesini sağlarsınız.
  tty.c_oflag &= ~OPOST;

  // Zaman aşımı ve minimum karakter sayısı (non-blocking read için)
  //   Control Characters (Kontrol Karakterleri) - VMIN ve VTIME gibi özel
  //   kontrol karakterlerinin değerlerini tutan dizi.
  tty.c_cc[VMIN] =
      1; // VMIN (Minimum karakter sayısı): read() fonksiyonunun geri dönmesi
         // için gereken minimum karakter sayısını belirtir.
  tty.c_cc[VTIME] = 255; // Zaman aşımını desisaniye (0.1 saniye) cinsinden
                         // belirtir. 255 değeri 25.5 saniye demektir.

  ret = tcsetattr(fd, TCSANOW, &tty);
  checkError(ret, "Error from tcsetattr");
}

void RSConfiguration::terminate() {
  close(fd);
  std::cout << "RS Channel File is closed successfully!" << std::endl;
}
