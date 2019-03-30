#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QtNetwork>

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
    unsigned long packetSize;
    unsigned char* pDataPacket;
};

#endif // MAINWINDOW_H
