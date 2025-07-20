#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "../configuration/include/configuration.hpp"
#include "../configuration/include/serialPortSettings.hpp"
#include "../receiver/include/receiver.hpp"
#include "../transmitter/include/transmitter.hpp"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots: // Functions that capture events write these blocks as slot functions.
    void on_receiver_start_clicked(); // on and clicked is a signal name
    void on_receiver_stop_clicked();
    void on_receiver_setDeviceConfiguration_clicked();
    void on_receiver_getDeviceConfiguration_clicked();
    void on_receiver_receiveMessages_clicked();

    void on_transmitter_start_clicked(); // on and clicked is a signal name
    void on_transmitter_stop_clicked();
    void on_transmitter_data_cellChanged(int row, int column); // İçerik değiştiğinde veri toplama ve doğrulama için
    void on_transmitter_sendHex_clicked();
    void on_transmitter_sendString_clicked();
    void on_transmitter_setDeviceConfiguration_clicked();
    void on_transmitter_getDeviceConfiguration_clicked();

private:
    Ui::MainWindow *ui;
    RSConfiguration *m_configReceive;
    RSConfiguration *m_configTransmit;
    RSReceive *m_receive;
    RSTransmit *m_transmit;
    SerialPortSettings *m_settingsReceive;
    SerialPortSettings *m_settingsTransmit;

    QTimer* m_receiveTimer; // Zamanlayıcı için bir pointer
    QString captureStdOut(const std::function<void()>& func);
    void checkForSerialMessages();

    void setupHexTable(); // Tabloyu ilk kuran fonksiyon
    void resetHexTable(); // Tabloyu ve veriyi sıfırlayan fonksiyon
    // O ana kadar birleştirilmiş hex string'ini tutar
    void updateConcatenatedHex();
    QString m_concatenatedHex;
};
#endif // MAINWINDOW_H
