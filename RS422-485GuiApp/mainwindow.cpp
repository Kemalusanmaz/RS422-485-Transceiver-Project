#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../configuration/include/serialPortSettings.hpp"
#include <iostream>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
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

    resetHexTable();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_configReceive;
    delete m_configTransmit;
    delete m_receive;
    delete m_transmit;
    delete m_settingsReceive;
    delete m_settingsTransmit;
}

QString MainWindow::captureStdOut(const std::function<void ()> &func)
{
    // buffer for cout
    std::streambuf* oldCoutStreamBuf = std::cout.rdbuf();

    // object to catching outputs
    std::ostringstream strCout;

    // forward cout outputs to stream object
    std::cout.rdbuf(strCout.rdbuf());

    try {
        func();
    } catch (...) {
        ui->receiver_logDisplay->appendPlainText("Unknown Error!");
    }
    //replace cout to original buffer
    std::cout.rdbuf(oldCoutStreamBuf);

    return QString::fromStdString(strCout.str());
}

void MainWindow::on_receiver_start_clicked()
{
    m_configReceive = new RSConfiguration();
    m_settingsReceive = new SerialPortSettings();

    QString output = captureStdOut([&]() {
        m_configReceive->initialize(m_configReceive->getJsonData()["receiverDeviceName"]);
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }

    m_receive = new RSReceive(m_configReceive->getFd());

    ui->receiver_stop->setEnabled(true);
    ui->receiver_setDeviceConfiguration->setEnabled(true);
    ui->receiver_start->setEnabled(false);
}

void MainWindow::on_receiver_stop_clicked()
{
    if (m_receiveTimer && m_receiveTimer->isActive()) {
        m_receiveTimer->stop();
    }

    QString output = captureStdOut([&]() {
        m_configReceive->terminate();
    });

    delete m_settingsReceive;

    ui->receiver_start->setEnabled(true);
    ui->receiver_stop->setEnabled(false);
    ui->receiver_setDeviceConfiguration->setEnabled(false);
    ui->receiver_getDeviceConfiguration->setEnabled(false);
    ui->receiver_receiveMessages->setEnabled(false);

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_receiver_setDeviceConfiguration_clicked()
{
    ui->receiver_receiveMessages->setEnabled(true);
    ui->receiver_getDeviceConfiguration->setEnabled(true);
    ui->receiver_setDeviceConfiguration->setEnabled(false);

    int baudrate = ui->receiver_baudrate->currentText().toInt();
    int dataBits = ui->receiver_dataBits->currentText().toInt();
    std::string parity= ui->receiver_parity->currentText().toStdString();
    int stopBits = ui->receiver_stopBits->currentText().toInt();

    m_settingsReceive->setBaudrate(baudrate);
    m_settingsReceive->setDataBits(dataBits);
    m_settingsReceive->setParity(parity);
    m_settingsReceive->setStopBits(stopBits);

    QString output = captureStdOut([&]() {
        m_configReceive->setRsConfig(*m_settingsReceive);
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_receiver_receiveMessages_clicked()
{
    ui->receiver_receiveMessages->setEnabled(false);
    // Create timer
    if (!m_receiveTimer) {
        m_receiveTimer = new QTimer(this);
        // connect the timer's timeout signal to the slot that controls messages.
        connect(m_receiveTimer, &QTimer::timeout, this, &MainWindow::checkForSerialMessages);
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

void MainWindow::checkForSerialMessages()
{
    if (!m_receive) return;
    QString output = captureStdOut([&]() {
        m_receive->receiveData(1024);
    });

    if (!output.isEmpty()) {
        ui->receiver_receiveMessagesDisplay->appendPlainText(output);
    }
}

void MainWindow::on_receiver_getDeviceConfiguration_clicked()
{
    QString output = captureStdOut([&]() {
        m_configReceive->getRsConfig(*m_settingsReceive);
    });

    if (!output.isEmpty()) {
        ui->receiver_logDisplay->appendPlainText(output);
    }

}

void MainWindow::on_transmitter_start_clicked()
{
    m_configTransmit= new RSConfiguration();
    m_settingsTransmit= new SerialPortSettings();

    QString output = captureStdOut([&]() {
        m_configTransmit->initialize(m_configTransmit->getJsonData()["transmitterDeviceName"]);
    });

    if (!output.isEmpty()) {
        ui->transmitter_logDisplay->appendPlainText(output);
    }

    m_transmit= new RSTransmit(m_configTransmit->getFd());
    m_transmit->sendData("\n");

    ui->transmitter_stop->setEnabled(true);
    ui->transmitter_setDeviceConfiguration->setEnabled(true);
    ui->transmitter_start->setEnabled(false);
}

void MainWindow::on_transmitter_stop_clicked()
{
    QString output = captureStdOut([&]() {
        m_configTransmit->terminate();
    });

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
}

void MainWindow::on_transmitter_setDeviceConfiguration_clicked()
{
    ui->transmitter_sendHex->setEnabled(true);
    ui->transmitter_sendString->setEnabled(true);
    ui->transmitter_getDeviceConfiguration->setEnabled(true);
    ui->transmitter_setDeviceConfiguration->setEnabled(false);

    int baudrate = ui->transmitter_baudrate->currentText().toInt();
    int dataBits = ui->transmitter_dataBits->currentText().toInt();
    std::string parity= ui->transmitter_parity->currentText().toStdString();
    int stopBits = ui->transmitter_stopBits->currentText().toInt();

    m_settingsTransmit->setBaudrate(baudrate);
    m_settingsTransmit->setDataBits(dataBits);
    m_settingsTransmit->setParity(parity);
    m_settingsTransmit->setStopBits(stopBits);

    QString output = captureStdOut([&]() {
        m_configTransmit->setRsConfig(*m_settingsTransmit);
    });

    if (!output.isEmpty()) {
        ui->transmitter_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_transmitter_getDeviceConfiguration_clicked()
{
    QString output = captureStdOut([&]() {
        m_configTransmit->getRsConfig(*m_settingsTransmit);
    });

    if (!output.isEmpty()) {
        ui->transmitter_logDisplay->appendPlainText(output);
    }
}

void MainWindow::on_transmitter_sendHex_clicked()
{
    // Check if the data to be sent is empty.
    if (m_concatenatedHex.isEmpty()) {
        return;
    }

    // data send processing
    m_concatenatedHex = m_concatenatedHex.remove(' ');
    std::string dataToSend = m_concatenatedHex.toStdString();

    QString output = captureStdOut([&]() {
        m_transmit->sendData(dataToSend);
    });

    if (!output.isEmpty()) {
        ui->transmitter_logDisplay->appendPlainText(output);
    }

    resetHexTable();
}

void MainWindow::on_transmitter_data_cellChanged(int row, int column)
{
    QTableWidgetItem* currentItem = ui->transmitter_data->item(row, column);
    if (!currentItem) return;

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

void MainWindow::updateConcatenatedHex()
{
    m_concatenatedHex.clear();
    QStringList hexParts; // A list to hold valid hex values
    bool dataEnded = false; // Flag that marks the end of data

    for (int row = 0; row < ui->transmitter_data->rowCount(); ++row) {
        for (int col = 0; col < ui->transmitter_data->columnCount(); ++col) {
            QTableWidgetItem* item = ui->transmitter_data->item(row, col);

            // If the cell is not empty and the text is valid hexadecimal
            if (item && item->text() != "-") {
                // If the data ran out before then, it is a "gap" error.
                if (dataEnded) {
                    m_concatenatedHex.clear(); // Clear string as it is incorrect
                    ui->transmitter_dataDisplay->setPlainText(m_concatenatedHex);
                    return;
                }
                hexParts.append(item->text());
            } else {
               // If the cell is blank or contains "-", assume the data ends there.
                dataEnded = true;
            }
        }
    }
    m_concatenatedHex = hexParts.join(" ");
    ui->transmitter_dataDisplay->setPlainText(m_concatenatedHex);
}

// This function initializes the table.
void MainWindow::setupHexTable()
{
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
void MainWindow::resetHexTable()
{
    m_concatenatedHex.clear();
    ui->transmitter_dataDisplay->clear();
    setupHexTable(); // Initializes the table
}

void MainWindow::on_transmitter_sendString_clicked()
{
    std::string stringData = ui->transmitter_stringData->toPlainText().toStdString();

    if(!stringData.empty()){
        QString output = captureStdOut([&]() {
            m_transmit->sendData(stringData);
        });

        if (!output.isEmpty()) {
            ui->transmitter_logDisplay->appendPlainText(output);
        }
    }
    ui->transmitter_stringData->clear();
}

