#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "srh_control_struc.h"

#include <CCfits>
using namespace CCfits;

//The main window of a srh36
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow){
    pCorrelatorClient = new QTcpSocket(this);
    QObject::connect(pCorrelatorClient, SIGNAL(connected()), this, SLOT(on_correlatorClient_connected()));
    QObject::connect(pCorrelatorClient, SIGNAL(readyRead()), this, SLOT(on_correlatorClient_read()));
    QObject::connect(pCorrelatorClient, SIGNAL(disconnected()), this, SLOT(on_correlatorClient_disconnected()));

    packetNumber = 0;
    packetSize = 0;
    pDataPacket = NULL;
    QSettings settings;
    correlatorIP = settings.value("network/correlatorIP", "10.0.5.179").toString();
    correlatorPort = settings.value("network/correlatorPort", 56565).toInt();

    ui->setupUi(this);
    ui->plotter->addGraph();
    ui->plotter->addGraph();
    ui->plotter->addGraph();
    ui->plotter->xAxis->setRange(0, 20);
    ui->plotter->yAxis->setRange(0, 20000);
}

MainWindow::~MainWindow(){
    QSettings settings;
    settings.setValue("network/correlatorIP", correlatorIP);
    settings.setValue("network/correlatorPort", correlatorPort);
    delete ui;
}

void MainWindow::on_writeButton_clicked(){
    unsigned long rows(2);
    unsigned long samples(100);
    unsigned long numberOfAntennas(8);
    string hduSign("SRH_DATA");
    const std::string fitsName("simpleBintable.fit");
    std::vector<string> colName(3,"");
    std::vector<string> colForm(3,"");
    std::vector<string> colUnit(3,"");

    colName[0] = "frequency";
    colName[1] = "time";
    colName[2] = "visibility";

    colForm[0] = "1J";
    colForm[1] = "100D";
    colForm[2] = "6400M";

    colUnit[0] = "Hz";
    colUnit[1] = "second";
    colUnit[2] = "correlation";

    std::vector<float> freqs(rows);
    freqs[0] = 4000.;
    freqs[1] = 6000.;

    std::valarray<double> fTime(samples);
    std::vector<std::valarray<double> > times(rows);
    for (unsigned int j = 0; j < rows; ++j){
        for (unsigned long i = 0; i < samples; ++i)
            fTime[i] = 0.1*j*i;
        times[j] = fTime;
    }

    std::valarray<std::complex<double> > ftVis(64*samples);
    std::vector<std::valarray<complex<double> > > vis(rows);
    for (unsigned int j = 0; j < rows; ++j){
        for (unsigned long i = 0; i < samples; ++i)
            for (unsigned long k = 0; k < numberOfAntennas; ++k)
                for (unsigned long l = 0; l < numberOfAntennas; ++l){
                    std::complex<double> Z(k*i, l);
                    ftVis[i*numberOfAntennas*numberOfAntennas + k*l] = Z;
                }
        vis[j] = ftVis;
    }

    try{
        FITS fitsTable(fitsName, Write);
        Table* pTable = fitsTable.addTable(hduSign, rows, colName, colForm, colUnit);
        pTable->column(colName[0]).write(freqs, 1);
        pTable->column(colName[1]).writeArrays(times, 1);
        pTable->column(colName[2]).writeArrays(vis, 1);
    }

    catch (FITS::CantCreate){
        QMessageBox::information(NULL, "info", "FITS already exists");
    }
}

void MainWindow::on_connectButton_clicked(bool checked){
    if (checked)
        pCorrelatorClient->connectToHost(correlatorIP, correlatorPort);
    else
        pCorrelatorClient->disconnectFromHost();
}

void MainWindow::on_correlatorClient_connected(){
    ui->logText->append(QString("connected"));
    ui->logText->append(QString::number(dHlgrph_PkgMaxSz));
    ui->logText->append(QString::number(sizeof(tPkg)));
}

void MainWindow::on_correlatorClient_disconnected(){
    ui->logText->append(QString("disconnected"));
}

void MainWindow::on_correlatorClient_read(){
    packetSize = pCorrelatorClient->bytesAvailable();
    tPkg correlatorPacket;
    if (packetSize >= sizeof(tPkg)){
        pCorrelatorClient->read(reinterpret_cast<char*>(&correlatorPacket), sizeof(tPkg));
        switch (correlatorPacket.H.Rqst){
        case eRqst_GetProperty:
            ui->logText->append(QString("eRqst_GetProperty"));
            ui->logText->append(QString(reinterpret_cast<const char*>(correlatorPacket.D.pU8)));
            break;
        case eRqst_SetStateProcessing:
            if (correlatorPacket.H.isRqstR)
                ui->logText->append("eRqst_SetStateProcessing OK");
            else
                ui->logText->append("eRqst_SetStateProcessing fault");
            break;
        case eRqst_Rdr_Strm_DtOut:
//            if (correlatorPacket.H.isPacked){
                if (correlatorPacket.D.Blck.DtBlck.Offset == 0){
                    ui->logText->append("First data packet");
                    if (pDataPacket)
                        delete pDataPacket;
                    pDataPacket = new unsigned char[correlatorPacket.D.Blck.DtBlck.FullDtSz];
                } else if (correlatorPacket.D.Blck.DtBlck.Offset + correlatorPacket.D.Blck.DtBlck.DtSz - correlatorPacket.D.Blck.DtBlck.FullDtSz == 0){
                    ui->logText->append("Last data packet");
                } else {
                    ui->logText->append("Data packet");
                }
                for (uint32_t i = 0;i < correlatorPacket.D.Blck.DtBlck.DtSz;++i)
                    pDataPacket[i + correlatorPacket.D.Blck.DtBlck.Offset] = correlatorPacket.D.pU8[i];
                ui->plotter->graph(0)->addData(packetNumber, *(reinterpret_cast<uint32_t*>(pDataPacket + correlatorPacket.D.Blck.DtBlck.Offset)));
//            }
            break;
        }
        ui->plotter->replot();
        ++packetNumber;
    }
}

void MainWindow::on_sendButton_clicked(){
    tPkg requestPacket;
    requestPacket.H.Rqst = eRqst_GetProperty;
    requestPacket.H.HeadExtTp = 0x00;
    requestPacket.H.Magic = 0x05;
    pCorrelatorClient->write((const char*)&requestPacket, sizeof(tPkg_Head));
}

void MainWindow::on_requestDataButton_clicked(){
    tPkg requestPacket;
    requestPacket.H.Rqst = eRqst_SetStateProcessing;
    requestPacket.H.HeadExtTp = 0x00;
    requestPacket.H.Magic = 0x05;
    pCorrelatorClient->write((const char*)&requestPacket, sizeof(tPkg_Head));
}
