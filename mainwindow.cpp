#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "srh_control_struc.h"

#include <CCfits>
using namespace CCfits;
#include <cstring>

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
    fitsDataOffset = 0;
    QSettings settings;
    correlatorIP = settings.value("network/correlatorIP", "10.0.5.179").toString();
    correlatorPort = settings.value("network/correlatorPort", 56565).toInt();
    fullPacketsInFits = settings.value("FITS/fullPacketsInFits", 256).toInt();
    frequencyListSize = settings.value("FITS/frequencyListSize", 32).toInt();
    numberOfVisibilities = 512;
    fullPacketNumber = 0;
    pDataPacket = new unsigned char[dHlgrph_PkgMaxSz * fullPacketsInFits * 2 * 4];

    freqColumn.resize(frequencyListSize);
    std::valarray<double> dArr(fullPacketsInFits / frequencyListSize);
    timeColumn.assign(frequencyListSize, dArr);
    std::valarray<complex<float> > cfArr(fullPacketsInFits / frequencyListSize * numberOfVisibilities);
    rcpVisColumn.assign(frequencyListSize, cfArr);
    lcpVisColumn.assign(frequencyListSize, cfArr);

    ui->setupUi(this);
    ui->plotter->addGraph();
    ui->plotter->addGraph();    ui->plotter->addGraph();
    ui->plotter->xAxis->setLabel("packet number");
    ui->plotter->yAxis->setLabel("data packet offset");
    ui->plotter->xAxis->setRange(0, 20);
    ui->plotter->yAxis->setRange(0, 10000);
    ui->plotter->graph(0)->setPen(QPen(QColor(255,0,0),1));
    ui->plotter->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssCross, 4));
}

MainWindow::~MainWindow(){
    QSettings settings;
    settings.setValue("network/correlatorIP", correlatorIP);
    settings.setValue("network/correlatorPort", correlatorPort);
    settings.setValue("FITS/fullPacketsInFits", fullPacketsInFits);
    settings.setValue("FITS/frequencyListSize", frequencyListSize);
    delete ui;
}

void MainWindow::on_writeButton_clicked(){
    unsigned long rows(2);
    unsigned long samples(100);
    unsigned long numberOfAntennas(8);
    string hduSign("SRH_DATA");
    QString qFitsName = "srh_" + QDateTime::currentDateTime().toString() + ".fit";
    const std::string fitsName = qFitsName.toStdString();
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
    QString msg;
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
            QTime curTime = QTime::currentTime();
            if (correlatorPacket.H.isPacked){
                fitsDataOffset = correlatorPacket.D.Blck.DtBlck.FullDtSz * fullPacketNumber;
                msg.sprintf("packet %d, packetInFits %d, f %d, t %f", fullPacketNumber, fullPacketsInFits, correlatorPacket.H.frequencyIndex, curTime.msecsSinceStartOfDay() * 0.001);
                ui->logText->append(msg);
                std::memcpy(pDataPacket + correlatorPacket.D.Blck.DtBlck.Offset + fitsDataOffset, correlatorPacket.D.pU8, correlatorPacket.D.Blck.DtBlck.DtSz);
//                for (uint32_t i = 0;i < correlatorPacket.D.Blck.DtBlck.DtSz;++i)
//                    pDataPacket[i + correlatorPacket.D.Blck.DtBlck.Offset + fitsDataOffset] = correlatorPacket.D.pU8[i];
                if (correlatorPacket.D.Blck.DtBlck.Offset == 0){
                    ui->logText->append("First data packet");
                } else if (correlatorPacket.D.Blck.DtBlck.Offset + correlatorPacket.D.Blck.DtBlck.DtSz == correlatorPacket.D.Blck.DtBlck.FullDtSz){
                    ui->logText->append("Last data packet");
                    if (correlatorPacket.H.frequencyIndex < frequencyListSize){
                        timeColumn[correlatorPacket.H.frequencyIndex][fullPacketNumber / frequencyListSize] = curTime.msecsSinceStartOfDay() * 0.001;
                        if (correlatorPacket.H.polarization == 0) for (int j = 0;j < numberOfVisibilities;++j)
                            lcpVisColumn[correlatorPacket.H.frequencyIndex][fullPacketNumber / frequencyListSize * numberOfVisibilities + j] = complex<float>(*(reinterpret_cast<uint32_t*>(pDataPacket) + j), 0.);
                        else for (int j = 0;j < numberOfVisibilities;++j)
                            rcpVisColumn[correlatorPacket.H.frequencyIndex][fullPacketNumber / frequencyListSize * numberOfVisibilities + j] = complex<float>(*(reinterpret_cast<uint32_t*>(pDataPacket) + j), 0.);
                    }
                    if (++fullPacketNumber >= fullPacketsInFits){
                        writeCurrentFits();
                        fullPacketNumber = 0;
                    }
                } else {
                    ui->logText->append("Data packet");
                }
                ui->plotter->graph(0)->addData(packetNumber, *(reinterpret_cast<uint32_t*>(pDataPacket + correlatorPacket.D.Blck.DtBlck.Offset + fitsDataOffset)));
            }
            break;
        }
        ++packetNumber;
        ui->plotter->graph(1)->addData(packetNumber, fullPacketNumber*100);
        ui->plotter->xAxis->setRange(0, packetNumber + 20);
        ui->plotter->replot();
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
    if (ui->requestDataButton->isChecked())
        requestPacket.H.Rqst = eRqst_SetStateProcessing;
    else
        requestPacket.H.Rqst = eRqst_SetStateIdle;
    requestPacket.H.HeadExtTp = 0x00;
    requestPacket.H.Magic = 0x05;
    pCorrelatorClient->write((const char*)&requestPacket, sizeof(tPkg_Head));
}

void MainWindow::writeCurrentFits(){
    unsigned long rows(frequencyListSize);
    string hduSign("SRH_DATA");
    QString qFitsName = "srh_" + QDateTime::currentDateTime().toString() + ".fit";
    const std::string fitsName = qFitsName.toStdString();
    std::vector<string> colName(4,"");
    std::vector<string> colForm(4,"");
    std::vector<string> colUnit(4,"");
    QString columnFormat;

    colName[0] = "frequency";
    colName[1] = "time";
    colName[2] = "lcpVis";
    colName[3] = "rcpVis";

    colForm[0] = "1J";
    columnFormat.sprintf("%dD", fullPacketsInFits / frequencyListSize); colForm[1] = columnFormat.toStdString();
    columnFormat.sprintf("%dC", fullPacketsInFits / frequencyListSize * numberOfVisibilities); colForm[2] = columnFormat.toStdString();
    columnFormat.sprintf("%dC", fullPacketsInFits / frequencyListSize * numberOfVisibilities); colForm[3] = columnFormat.toStdString();

    colUnit[0] = "Hz";
    colUnit[1] = "second";
    colUnit[2] = "correlation";
    colUnit[3] = "correlation";

    std::vector<float> freqs(rows);
    for (int i = 0;i < (int)rows;++i)
        freqs[i] = 4000. + i*500.;

    try{
        FITS fitsTable(fitsName, Write);
        Table* pTable = fitsTable.addTable(hduSign, rows, colName, colForm, colUnit);
        pTable->column(colName[0]).write(freqColumn, 1);
        pTable->column(colName[1]).writeArrays(timeColumn, 1);
        pTable->column(colName[2]).writeArrays(lcpVisColumn, 1);
        pTable->column(colName[3]).writeArrays(rcpVisColumn, 1);

    }

    catch (FITS::CantCreate){
        QMessageBox::information(NULL, "info", "FITS already exists");
    }
}

