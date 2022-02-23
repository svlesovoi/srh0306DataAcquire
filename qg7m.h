#ifndef QG7M_H
#define QG7M_H

#include "visa.h"
#include <QString>
#include <QStringList>

class QG7M{
    ViStatus status;
    ViSession defaultRsrcMngr;
    ViSession g7m;
    QString g7mName;
    QString listCmd;
public:
    QG7M(QString);
    QString startFrequencyList(QStringList);
    QString stopFrequencyList();
    QString getFrequencyList();
    QString getStatus();
};

#endif // QG7M_H
