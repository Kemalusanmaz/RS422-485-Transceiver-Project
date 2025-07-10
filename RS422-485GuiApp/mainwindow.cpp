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
    ui->transmitter_send->setEnabled(false);
    ui->transmitter_getDeviceConfiguration->setEnabled(false);
    ui->transmitter_setDeviceConfiguration->setEnabled(false);
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
{    // Butonun Start/Stop işlevi görmesini sağlayalım.
    ui->receiver_receiveMessages->setEnabled(false);
    // Eğer zamanlayıcı zaten oluşturulmadıysa, oluşturalım.
    if (!m_receiveTimer) {
        m_receiveTimer = new QTimer(this);
        // Zamanlayıcının timeout sinyalini, mesajları kontrol eden slotumuza bağlıyoruz.
        connect(m_receiveTimer, &QTimer::timeout, this, &MainWindow::checkForSerialMessages);
    }

    // Zamanlayıcı çalışıyor mu?
    if (m_receiveTimer->isActive()) {
        // --- STOP ---
        m_receiveTimer->stop();

        // Diğer butonları tekrar aktif et
        ui->receiver_setDeviceConfiguration->setEnabled(true);
    } else {
        // Diğer butonları pasif yap
        ui->receiver_setDeviceConfiguration->setEnabled(false);

        // Zamanlayıcıyı her 100 milisaniyede bir çalışacak şekilde başlat.
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



