#include "qg7m.h"

QG7M::QG7M(QString g7mIpAddress){
    viOpenDefaultRM(&defaultRsrcMngr);
    g7mName = "TCPIP::" + g7mIpAddress + "::8888::SOCKET::GNM";
}

QString QG7M::startFrequencyList(QStringList freqList){
    QString statusString;
    char buff[128];
    status = viOpen(defaultRsrcMngr, (char*)g7mName.toStdString().data(), VI_EXCLUSIVE_LOCK,5000, &g7m);
    if (status == VI_SUCCESS){
        listCmd = "LIST:FREQ ";
        for (int i = 0;i < freqList.size();++i){
            listCmd += freqList.at(i);
            if (i < freqList.size() - 1)
                listCmd += ",";
            else
                listCmd += "\n";
        }
        viSetAttribute(g7m,VI_ATTR_TMO_VALUE, 20000);
        viPrintf(g7m,ViString("INIT:CONT OFF\n"));                      viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);        statusString += "INIT:CONT" + QString(buff);
        viPrintf(g7m,ViString("FREQ:MODE LIST\n"));                     viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);        statusString += "FREQ:MODE LIST" + QString(buff);
        viPrintf(g7m,ViString(listCmd.toStdString().data()));           viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);        statusString += "LIST:FREQ" + QString(buff);
        viPrintf(g7m,ViString("TRIG:SEQ:SLOP NEG\n"));                  viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);        statusString += "TRIG SLOPE " + QString(buff);
        viPrintf(g7m,ViString("TRIG:SEQ:SOUR EXT\n"));                  viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);        statusString += "TRIG EXT " + QString(buff);
        viPrintf(g7m,ViString("LIST:MODE MAN\n"));                      viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);        statusString += "LIST MAN " + QString(buff);
        viPrintf(g7m,ViString("POW:AMPL 10 DBM\n"));                    viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);        statusString += "POW:AMPL " + QString(buff);
        viPrintf(g7m,ViString("POW:MODE FIX\n"));                       viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);        statusString += "POW:MODE " + QString(buff);
        viPrintf(g7m,ViString("OUTP:STAT ON\n"));                       viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);        statusString += "OUTP:STAT " + QString(buff);
        viPrintf(g7m,ViString("INITiate\n"));                           viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);        statusString += "INITiate " + QString(buff);
        viPrintf(g7m,ViString("*OPC?\n"));                              viScanf(g7m, ViString("%T"), buff);                                 statusString += "OPC " + QString(buff);

        viClose(g7m);
/*
            viPrintf(rsrc,"INIT:CONT OFF\n");                       status = viQueryf(rsrc, "SYST:ERR?\n", "%T", buff);            std::cout << "INIT CONT OFF Error: " << buff;
            viPrintf(rsrc, "FREQ:MODE LIST\n");                     status = viQueryf(rsrc, "SYST:ERR?\n", "%T", buff);            std::cout << "FREQ:MODE LIST: " << buff;
            viPrintf(rsrc, "LIST:FREQ 2800 MHz,3100 MHz,3400 MHz,3900 MHz,4700 MHz,5600 MHz\n");            status = viQueryf(rsrc, "SYSTem:ERR?\n", "%T", buff);            std::cout << "LIST:FREQ Error: " << buff;
            viPrintf(rsrc,"TRIG:SEQ:SLOP NEG\n");                   status = viQueryf(rsrc, "SYST:ERR?\n", "%T", buff);            std::cout << "TRIG SLOPE Error: " << buff;
            viPrintf(rsrc,"TRIG:SEQ:SOUR EXT\n");                   status = viQueryf(rsrc, "SYST:ERR?\n", "%T", buff);            std::cout << "TRIG EXT Error: " << buff;
            viPrintf(rsrc,"LIST:MODE MAN\n");                       status = viQueryf(rsrc, "SYST:ERR?\n", "%T", buff);            std::cout << "LIST MAN Error: " << buff;
            viPrintf(rsrc,"POW:AMPL 10 DBM\n");                     status = viQueryf(rsrc, "SYST:ERR?\n", "%T", buff);            std::cout << "POW:AMPL Error: " << buff;
            viPrintf(rsrc,"POW:MODE FIX\n");                        status = viQueryf(rsrc, "SYST:ERR?\n", "%T", buff);            std::cout << "POW:MODE Error: " << buff;
            viPrintf(rsrc,"OUTP:STAT ON\n");                        status = viQueryf(rsrc, "SYST:ERR?\n", "%T", buff);            std::cout << "OUTP:STAT ON Error: " << buff;
            viPrintf(rsrc,"INITiate\n");                            status = viQueryf(rsrc, "SYST:ERR?\n", "%T", buff);            std::cout << "INITiate Error: " << buff;
            viPrintf(rsrc, "*OPC?\n");                              status = viScanf(rsrc, "%T", buff);

            viClose(rsrc);
*/

    } else
           statusString = "startFrequencyList::Connection error";
    return statusString;
}

QString QG7M::stopFrequencyList(){
    QString statusString;
    char buff[128];
    status = viOpen(defaultRsrcMngr, (char*)g7mName.toStdString().data(), VI_EXCLUSIVE_LOCK,5000, &g7m);
    if (status == VI_SUCCESS){
        viSetAttribute(g7m,VI_ATTR_TMO_VALUE, 20000);
        viPrintf(g7m,ViString("*RST\n"));               viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);   statusString += "*RST " + QString(buff);
        viPrintf(g7m,ViString("OUTP:STAT OFF\n"));      viQueryf(g7m,ViString("SYST:ERR?\n"), ViString("%T"), buff);   statusString += "OUTP:STAT OFF " + QString(buff);

        viClose(g7m);
    } else
        statusString = "stopFrequencyList::Connection error";
    return statusString;
}

QString QG7M::getStatus(){
    return g7mName + " " + QString::number(status, 16);
}

QString QG7M::getFrequencyList(){
    return listCmd;
}
