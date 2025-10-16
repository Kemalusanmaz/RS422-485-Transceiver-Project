#include "mainwindow.h"
#include "../configuration/include/serialPortSettings.hpp"
#include "ui_mainwindow.h"
#include <QTimer>
#include <iostream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  ui->receiver_stop->setEnabled(false);
  ui->receiver_receiveMessages->setEnabled(false);
  ui->receiver_getDeviceConfiguration->setEnabled(false);
  ui->receiver_setDeviceConfiguration->setEnabled(false);

  ui->transmitter_stop->setEnabled(false);
  ui->transmitter_sendHex->setEnabled(false);
  ui->transmitter_sendString->setEnabled(false);
  ui->transmitter_getDeviceConfiguration->setEnabled(false);
  ui->transmitter_setDeviceConfiguration->setEnabled(false);

  m_transmitTimer = new QTimer(this);
  connect(m_transmitTimer, &QTimer::timeout, this,
          &MainWindow::on_transmitter_sendPeriodically);

  resetHexTable();
}

MainWindow::~MainWindow() {
  delete ui;
  delete m_configReceive;
  delete m_configTransmit;
  delete m_receive;
  delete m_transmit;
  delete m_settingsReceive;
  delete m_settingsTransmit;
}

QString MainWindow::captureStdOut(const std::function<void()> &func) {
  // buffer for cout
  std::streambuf *oldCoutStreamBuf = std::cout.rdbuf();

  // object to catching outputs
  std::ostringstream strCout;

  // forward cout outputs to stream object
  std::cout.rdbuf(strCout.rdbuf());

  try {
    func();
  } catch (...) {
    ui->receiver_logDisplay->appendPlainText("Unknown Error!");
  }
  // replace cout to original buffer
  std::cout.rdbuf(oldCoutStreamBuf);

  return QString::fromStdString(strCout.str());
}

void MainWindow::on_receiver_start_clicked() {
  m_configReceive = new RSConfiguration();
  m_settingsReceive = new SerialPortSettings();

  QString output = captureStdOut([&]() {
    m_configReceive->initialize(
        ui->receiver_channel->currentText().toStdString());
  });

  if (!output.isEmpty()) {
    ui->receiver_logDisplay->appendPlainText(output);
  }

  m_receive = new RSReceive(m_configReceive->getFd());
  ui->receiver_channel->setEnabled(false);
  ui->receiver_stop->setEnabled(true);
  ui->receiver_setDeviceConfiguration->setEnabled(true);
  ui->receiver_start->setEnabled(false);
}

void MainWindow::on_receiver_stop_clicked() {
  if (m_receiveTimer && m_receiveTimer->isActive()) {
    m_receiveTimer->stop();
  }

  QString output = captureStdOut([&]() { m_configReceive->terminate(); });

  delete m_settingsReceive;

  ui->receiver_channel->setEnabled(true);
  ui->receiver_start->setEnabled(true);
  ui->receiver_stop->setEnabled(false);
  ui->receiver_setDeviceConfiguration->setEnabled(false);
  ui->receiver_getDeviceConfiguration->setEnabled(false);
  ui->receiver_receiveMessages->setEnabled(false);
  ui->receiver_baudrate->setEnabled(true);
  ui->receiver_dataBits->setEnabled(true);
  ui->receiver_parity->setEnabled(true);
  ui->receiver_stopBits->setEnabled(true);

  if (!output.isEmpty()) {
    ui->receiver_logDisplay->appendPlainText(output);
  }
}

void MainWindow::on_receiver_setDeviceConfiguration_clicked() {
  ui->receiver_receiveMessages->setEnabled(true);
  ui->receiver_getDeviceConfiguration->setEnabled(true);
  ui->receiver_setDeviceConfiguration->setEnabled(false);

  int baudrate = ui->receiver_baudrate->currentText().toInt();
  int dataBits = ui->receiver_dataBits->currentText().toInt();
  std::string parity = ui->receiver_parity->currentText().toStdString();
  int stopBits = ui->receiver_stopBits->currentText().toInt();
  std::string channel = ui->receiver_channel->currentText().toStdString();

  m_settingsReceive->setBaudrate(baudrate);
  m_settingsReceive->setDataBits(dataBits);
  m_settingsReceive->setParity(parity);
  m_settingsReceive->setStopBits(stopBits);
  m_settingsReceive->setChannel(channel);

  QString output = captureStdOut(
      [&]() { m_configReceive->setRsConfig(*m_settingsReceive); });

  if (!output.isEmpty()) {
    ui->receiver_logDisplay->appendPlainText(output);
  }

  ui->receiver_baudrate->setEnabled(false);
  ui->receiver_dataBits->setEnabled(false);
  ui->receiver_parity->setEnabled(false);
  ui->receiver_stopBits->setEnabled(false);
}

void MainWindow::on_receiver_receiveMessages_clicked() {
  ui->receiver_receiveMessages->setEnabled(false);
  // Create timer
  if (!m_receiveTimer) {
    m_receiveTimer = new QTimer(this);
    // connect the timer's timeout signal to the slot that controls messages.
    connect(m_receiveTimer, &QTimer::timeout, this,
            &MainWindow::checkForSerialMessages);
  }

  // If timer is working
  if (m_receiveTimer->isActive()) {
    // stop
    m_receiveTimer->stop();

    // activate buttons
    ui->receiver_setDeviceConfiguration->setEnabled(true);
  } else {
    // deactivate buttons
    ui->receiver_setDeviceConfiguration->setEnabled(false);

    // Start the timer to run every 100 milliseconds.
    m_receiveTimer->start(100);
  }
}

void MainWindow::checkForSerialMessages() {
  if (!m_receive)
    return;
  QString output;
  if (ui->receiver_readMode->currentText().toStdString() == "ASCII") {
    output = captureStdOut([&]() { m_receive->receiveData(1024); });
    if (!output.isEmpty()) {
      ui->receiver_receiveMessagesDisplay->appendPlainText(output);
    }
  } else if (ui->receiver_readMode->currentText().toStdString() == "HEX") {

     // Ham veriyi okusun ve dahili buffer'a koysun
      output = captureStdOut([&]() {     m_receive->receiveDataHex(
                                        1024); });
      if (!output.isEmpty()) {
          ui->receiver_receiveMessagesDisplay->appendPlainText(output);
      }
    // Şimdi m_receive'ın internalHexBuffer'ını alıp ekranda hex olarak
    // gösterelim
    const std::vector<uint8_t> &hexBuffer = m_receive->getReceivedHexBuffer();
    if (!hexBuffer.empty()) {
      QString hexString;
      for (uint8_t byte : hexBuffer) {
        hexString += QString("%1 ").arg((int)byte, 2, 16, QChar('0')).toUpper();
        //ui->receiver_receiveMessagesDisplay->appendPlainText(hexString);
      }
    }
     return; // Hex okumasında captureStdOut doğrudan string çıktısı
    // vermeyecektir.
  }
}

void MainWindow::on_receiver_getDeviceConfiguration_clicked() {
  QString output = captureStdOut(
      [&]() { m_configReceive->getRsConfig(*m_settingsReceive); });

  if (!output.isEmpty()) {
    ui->receiver_logDisplay->appendPlainText(output);
  }
}

void MainWindow::on_transmitter_start_clicked() {
  m_configTransmit = new RSConfiguration();
  m_settingsTransmit = new SerialPortSettings();

  QString output = captureStdOut([&]() {
    m_configTransmit->initialize(
        ui->transmitter_channel->currentText().toStdString());
  });

  if (!output.isEmpty()) {
    ui->transmitter_logDisplay->appendPlainText(output);
  }

  m_transmit = new RSTransmit(m_configTransmit->getFd());
  // m_transmit->sendData("\n");

  ui->transmitter_channel->setEnabled(false);
  ui->transmitter_stop->setEnabled(true);
  ui->transmitter_setDeviceConfiguration->setEnabled(true);
  ui->transmitter_start->setEnabled(false);
}

void MainWindow::on_transmitter_stop_clicked() {
  if (m_transmitTimer->isActive()) {
    m_transmitTimer->stop();
    // m_dataToSendPeriodically.clear();
    m_hexDataToSendPeriodically.clear();    // Hex verisini temizle
    m_stringDataToSendPeriodically.clear(); // String verisini temizle
  }

  QString output = captureStdOut([&]() { m_configTransmit->terminate(); });

  delete m_settingsTransmit;

  ui->transmitter_start->setEnabled(true);
  ui->transmitter_stop->setEnabled(false);
  ui->transmitter_setDeviceConfiguration->setEnabled(false);
  ui->transmitter_getDeviceConfiguration->setEnabled(false);
  ui->transmitter_sendHex->setEnabled(false);
  ui->transmitter_sendString->setEnabled(false);

  if (!output.isEmpty()) {
    ui->transmitter_logDisplay->appendPlainText(output);
  }

  ui->transmitter_channel->setEnabled(true);
  ui->receiver_baudrate->setEnabled(true);
  ui->receiver_dataBits->setEnabled(true);
  ui->receiver_parity->setEnabled(true);
  ui->receiver_stopBits->setEnabled(true);
}

void MainWindow::on_transmitter_setDeviceConfiguration_clicked() {
  ui->transmitter_sendHex->setEnabled(true);
  ui->transmitter_sendString->setEnabled(true);
  ui->transmitter_getDeviceConfiguration->setEnabled(true);
  ui->transmitter_setDeviceConfiguration->setEnabled(false);

  int baudrate = ui->transmitter_baudrate->currentText().toInt();
  int dataBits = ui->transmitter_dataBits->currentText().toInt();
  std::string parity = ui->transmitter_parity->currentText().toStdString();
  int stopBits = ui->transmitter_stopBits->currentText().toInt();
  std::string channel = ui->transmitter_channel->currentText().toStdString();

  m_settingsTransmit->setBaudrate(baudrate);
  m_settingsTransmit->setDataBits(dataBits);
  m_settingsTransmit->setParity(parity);
  m_settingsTransmit->setStopBits(stopBits);
  m_settingsTransmit->setChannel(channel);

  QString output = captureStdOut(
      [&]() { m_configTransmit->setRsConfig(*m_settingsTransmit); });

  if (!output.isEmpty()) {
    ui->transmitter_logDisplay->appendPlainText(output);
  }

  ui->transmitter_baudrate->setEnabled(false);
  ui->transmitter_dataBits->setEnabled(false);
  ui->transmitter_parity->setEnabled(false);
  ui->transmitter_stopBits->setEnabled(false);
}

void MainWindow::on_transmitter_getDeviceConfiguration_clicked() {
  QString output = captureStdOut(
      [&]() { m_configTransmit->getRsConfig(*m_settingsTransmit); });

  if (!output.isEmpty()) {
    ui->transmitter_logDisplay->appendPlainText(output);
  }
}
//||
void MainWindow::on_transmitter_sendPeriodically() {
  if (!m_transmit ) {
        if (m_currentSendType == SEND_STRING) {
          if(!m_stringDataToSendPeriodically.empty()){
                return;
          }
        }else if(m_currentSendType == SEND_HEX){
            if(!m_hexDataToSendPeriodically.empty()){
                return;
            }
        }
  }

  // QString output =
  //   captureStdOut([&]() { m_transmit->sendData(m_dataToSendPeriodically); });

  QString output;
  if (m_currentSendType == SEND_HEX) {
    if (m_hexDataToSendPeriodically.empty())
      return;
    output = captureStdOut(
        [&]() { m_transmit->sendDataHex(m_hexDataToSendPeriodically); });
  } else if (m_currentSendType == SEND_STRING) {
    if (m_stringDataToSendPeriodically.empty())
      return;
    output = captureStdOut(
        [&]() { m_transmit->sendData(m_stringDataToSendPeriodically); });
  }

  if (!output.isEmpty()) {
    ui->transmitter_logDisplay->appendPlainText(output);
  }
}

void MainWindow::on_transmitter_sendPeriodically_clicked(bool checked) {
  if (!checked && m_transmitTimer->isActive()) {
    m_transmitTimer->stop();
    m_stringDataToSendPeriodically.clear();
    m_hexDataToSendPeriodically.clear();
    ui->transmitter_sendHex->setEnabled(true);
    ui->transmitter_sendString->setEnabled(true);
  }
}

void MainWindow::on_transmitter_sendHex_clicked() {
  // Check if the data to be sent is empty.
  if (m_currentHexData.empty()) {
    return;
  }

  // data send processing
  // m_concatenatedHex = m_concatenatedHex.remove(' ');
  // std::string dataToSend = m_concatenatedHex.toStdString();

  if (ui->transmitter_sendPeriodically->isChecked()) {
    m_hexDataToSendPeriodically = m_currentHexData;
    m_currentSendType = SEND_HEX; // Hex gönderileceğini belirt

    int intervalMs = ui->transmitter_cycleTime->value();
    m_transmitTimer->start(intervalMs);

    ui->transmitter_sendHex->setEnabled(false);
    ui->transmitter_sendString->setEnabled(false);
  } else {
    QString output =
        captureStdOut([&]() { m_transmit->sendDataHex(m_currentHexData); });

    if (!output.isEmpty()) {
      ui->transmitter_logDisplay->appendPlainText(output);
    }
  }
  resetHexTable();
}

void MainWindow::on_transmitter_data_cellChanged(int row, int column) {
  QTableWidgetItem *currentItem = ui->transmitter_data->item(row, column);
  if (!currentItem)
    return;

  // STemporarily turn off signals to prevent signal looping
  ui->transmitter_data->blockSignals(true);

  QString originalText = currentItem->text();
  QString formattedText = originalText.toUpper().trimmed();

  // If the user deleted the entire cell or there was only space
  if (formattedText.isEmpty()) {
    // Set the cell to its default "blank" state
    currentItem->setText("-");
  }
  // If there is a valid hex input
  else {
    QRegularExpression hexMatcher("^[0-9A-F]{1,2}$");
    if (hexMatcher.match(formattedText).hasMatch()) {
      // If only one character is entered, add '0' at the beginning
      if (formattedText.length() == 1) {
        formattedText = "0" + formattedText;
      }
      // Write back formatted text to the cell (capitalization, etc.)
      currentItem->setText(formattedText);
    }
    // If the input is invalid (neither empty nor valid hex)
    else {
      // revert the cell as - char.
      currentItem->setText("-");
    }
  }
  // Recreate the data string after the change
  updateConcatenatedHex();

  // open signals
  ui->transmitter_data->blockSignals(false);
}

void MainWindow::updateConcatenatedHex() {
  m_currentHexData.clear();
  QStringList hexParts;   // A list to hold valid hex values
  bool dataEnded = false; // Flag that marks the end of data

  for (int row = 0; row < ui->transmitter_data->rowCount(); ++row) {
    for (int col = 0; col < ui->transmitter_data->columnCount(); ++col) {
      QTableWidgetItem *item = ui->transmitter_data->item(row, col);

      // If the cell is not empty and the text is valid hexadecimal
      if (item && item->text() != "-") {
        // If the data ran out before then, it is a "gap" error.
        if (dataEnded) {
          m_currentHexData.clear(); // Clear string as it is incorrect
          hexParts.clear();
          ui->transmitter_dataDisplay->setPlainText("");
          return;
        }
        hexParts.append(item->text());
        bool ok;
        unsigned int val = item->text().toUInt(&ok, 16);
        if (ok && val <= 0xFF) { // 0xFF (255) değeri, bir uint8_t'nin
                                 // alabileceği maksimum değerdir.
          m_currentHexData.push_back(
              static_cast<uint8_t>(val)); // uint8_t'ye cast et
          hexParts.append(item->text());  // Ekran için string listesine ekle
        } else {
          // Eğer dönüşümde hata olursa (geçersiz hex)
          m_currentHexData.clear();
          hexParts.clear();
          ui->transmitter_dataDisplay->setPlainText("Invalid Hex Input!");
          return;
        }
      } else {
        // If the cell is blank or contains "-", assume the data ends there.
        dataEnded = true;
      }
    }
  }
  // m_concatenatedHex = hexParts.join(" ");
  ui->transmitter_dataDisplay->setPlainText("");
}

// This function initializes the table.
void MainWindow::setupHexTable() {
  ui->transmitter_data->blockSignals(true);
  for (int row = 0; row < 8; ++row) {
    for (int col = 0; col < 8; ++col) {
      QTableWidgetItem *item = new QTableWidgetItem("-");
      item->setTextAlignment(Qt::AlignCenter);
      ui->transmitter_data->setItem(row, col, item);
    }
  }
  ui->transmitter_data->blockSignals(false);
}

// This function resets both the table and the data variables.
void MainWindow::resetHexTable() {
  // m_concatenatedHex.clear();
  m_currentHexData.clear(); // m_currentHexData vektörünü temizle
  ui->transmitter_dataDisplay->clear();
  setupHexTable(); // Initializes the table
}

void MainWindow::on_transmitter_sendString_clicked() {
  std::string stringData =
      ui->transmitter_stringData->toPlainText().toStdString();
  if (ui->transmitter_sendPeriodically->isChecked()) {
    m_stringDataToSendPeriodically = stringData;
    m_currentSendType = SEND_STRING; // String gönderileceğini belirt
    int intervalMs = ui->transmitter_cycleTime->value();
    m_transmitTimer->start(intervalMs);

    ui->transmitter_sendHex->setEnabled(false);
    ui->transmitter_sendString->setEnabled(false);
  } else {
    if (!stringData.empty()) {
      QString output =
          captureStdOut([&]() { m_transmit->sendData(stringData); });

      if (!output.isEmpty()) {
        ui->transmitter_logDisplay->appendPlainText(output);
      }
    }
  }
  ui->transmitter_stringData->clear();
}
