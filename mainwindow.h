#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtNetwork>
#include <QString>
#include <valarray>
#include <complex>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_writeButton_clicked();

    void on_connectButton_clicked(bool checked);

    void on_sendButton_clicked();

    void on_correlatorClient_connected();
    void on_correlatorClient_disconnected();
    void on_correlatorClient_read();

    void on_requestDataButton_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket* pCorrelatorClient;
    QString correlatorIP;
    int correlatorPort;
    int packetNumber;
    int lcpFullPacketNumber;
    int rcpFullPacketNumber;
    int lcpFitsDataOffset;
    int rcpFitsDataOffset;
    unsigned long packetSize;
    unsigned char* lcpDataPacket;
    unsigned char* rcpDataPacket;

    int frequencyListSize;
    int fullPacketsInFits;
    int numberOfVisibilities;

    std::vector<float> freqColumn;
    std::vector<std::valarray<double> > timeColumn;
    std::vector<std::valarray<std::complex<float> > > rcpVisColumn;
    std::vector<std::valarray<std::complex<float> > > lcpVisColumn;

    void writeCurrentFits();
};

#endif // MAINWINDOW_H
