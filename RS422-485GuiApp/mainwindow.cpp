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

    // 1. Gönderilecek verinin boş olup olmadığını kontrol et.
    if (m_concatenatedHex.isEmpty()) {
        return;
    }

    // 2. VERİYİ GÖNDERME İŞLEMİ
    // Burada, m_concatenatedHex string'ini alıp
    // m_transmitter nesnenizin sendMessage fonksiyonuna gönderirsiniz.
    // m_concatenatedHex "AA BB CC" formatında olduğu için önce boşlukları temizlemeniz gerekebilir.
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

    // Sinyal döngüsünü engellemek için sinyalleri geçici olarak kapat
    ui->transmitter_data->blockSignals(true);

    QString originalText = currentItem->text();
    QString formattedText = originalText.toUpper().trimmed();

    // 1. Kullanıcı hücreyi tamamen sildiyse veya sadece boşluk varsa
    if (formattedText.isEmpty()) {
        // Hücreyi varsayılan "boş" durumuna getir
        currentItem->setText("-");
    }
    // 2. Eğer geçerli bir hex girişi varsa
    else {
        QRegularExpression hexMatcher("^[0-9A-F]{1,2}$");
        if (hexMatcher.match(formattedText).hasMatch()) {
            // Eğer tek karakter girildiyse, başına '0' ekle
            if (formattedText.length() == 1) {
                formattedText = "0" + formattedText;
            }
            // Hücreye formatlanmış metni geri yaz (büyük harf vb.)
            currentItem->setText(formattedText);
        }
        // 3. Eğer giriş geçersizse (ne boş ne de geçerli hex)
        else {
            // Hatalı girişi geri al ve kullanıcıyı uyar
            // Hatalı giriş öncesi hücrenin durumuna geri dönmek zor olabilir.
            // En basit çözüm, onu 'boş' durumuna getirmektir.
            currentItem->setText("-");
        }
    }

    // Her durumda, değişiklikten sonra veri string'ini yeniden oluştur.
    updateConcatenatedHex();

    // Sinyalleri tekrar aç
    ui->transmitter_data->blockSignals(false);
}

void MainWindow::updateConcatenatedHex()
{
    m_concatenatedHex.clear();
    QStringList hexParts; // Geçerli hex değerlerini tutacak bir liste

    bool dataEnded = false; // Verinin bittiği yeri işaretleyen bayrak
    for (int row = 0; row < ui->transmitter_data->rowCount(); ++row) {
        for (int col = 0; col < ui->transmitter_data->columnCount(); ++col) {
            QTableWidgetItem* item = ui->transmitter_data->item(row, col);

            // Eğer hücre boş değilse ve metin geçerli bir hex ise
            if (item && item->text() != "-") {
                // Eğer daha önce veri bitmişse, bu bir "aradaki boşluk" hatasıdır.
                if (dataEnded) {
                    m_concatenatedHex.clear(); // Hatalı olduğu için string'i temizle
                    ui->transmitter_dataDisplay->setPlainText(m_concatenatedHex);
                    return; // Fonksiyondan çık
                }
                hexParts.append(item->text());
            } else {
                // Hücre boşsa veya "-" içeriyorsa, verinin burada bittiğini varsay.
                dataEnded = true;
            }
        }
    }

    m_concatenatedHex = hexParts.join(" ");
    ui->transmitter_dataDisplay->setPlainText(m_concatenatedHex);
}

// Bu fonksiyon tabloyu ilk haline getirir.
void MainWindow::setupHexTable()
{
    ui->transmitter_data->blockSignals(true);
    // ... (satır/sütun ayarlama kısmı aynı) ...
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            // Başlangıçta "-" veya boş bırakmak daha anlamlı
            QTableWidgetItem *item = new QTableWidgetItem("-");
            item->setTextAlignment(Qt::AlignCenter);
            ui->transmitter_data->setItem(row, col, item);
        }
    }
    ui->transmitter_data->blockSignals(false);
}

void MainWindow::resetHexTable()
{
    m_concatenatedHex.clear();
    ui->transmitter_dataDisplay->clear();
    setupHexTable(); // Tabloyu ilk haline getirir
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

