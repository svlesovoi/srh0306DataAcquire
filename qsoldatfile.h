/***************************************************************************
 *   Copyright (C) 2008 by Sergey V. Lesovoi,,+7-3952-511841,+7-3952-511214   *
 *   sergey@ostrich   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef STDQSOLDATFILE_H
#define STDQSOLDATFILE_H

#include <qfile.h>
#include <qdatetime.h>

//namespace std {

/**
    @author Sergey V. Lesovoi,+7-914-88-77-965, svlesovoi@gmail.com
*/
	class QSoldatFile : public QFile{
public:
	QSoldatFile();
	~QSoldatFile();
	bool fromFile(QString&);

	bool   setDate(QDate&);
	bool   setTime(double);
	bool   IsValidTime();
	bool   IsValidCurrentDay();
	double arcminToRad(double);
	double radToArcmin(double);
	double radToArcsec(double);
	double arcsecToRad(double);
	double degToRad(double);
	double radToDeg(double);

	QString Culmination();
	QString Declination();
	QString Radius();
	QString PAngle();
	QString BAngle();
	QString LAngle();
	QString DDdt();
	
	double	DRadius();
	double	DCulmination();
	double	DDeclination();
	double	DPAngle();
	double	DBAngle();
	double	DDDdt();
	double	DLAngle();

	int    EastWestOrder(double t = -10.e6);
	int    NorthSouthOrder(double t = -10.e6);
	double TDDeclination();
	double TDHourAngle();
	int    TEastWestOrder(double frequency);
	int    TNorthSouthOrder(double frequency);

	double OP(double frequency);
	double OQ(double frequency);

	double TP();
	double TQ();

	double TDEastWestBeamAngle();
	double TDNorthSouthBeamAngle();
	double TDEastWestBeamWidth();
	double TDNorthSouthBeamWidth();

	QString EarthRotationPeriod();
	double  DEarthRotationPeriod();

	double DDHdt();
	double DD2Ddt2();
	double DD2Hdt2();

	QString getSolarDataTitle();
	void    setSolarDataTitle(const char* pTitle);
	int     getCurrentEastWestOrder();
	void    setCurrentEastWestOrder(int ewOrder);
	int     getCurrentNorthSouthOrder();
	void    setCurrentNorthSouthOrder(int nsOrder);

	double  delayForPair(int A1, int A2);
	double  delayForEwPair(int A1, int A2);
	double  delayForNsPair(int A1, int A2);
	double  delayForAntenna(int A, double hourAngle, double decliantion);
	bool  	uvwForPair(int A1, int A2, double& u, double& v, double& w);

	void	hor2eq(double altitude, double azimuth, double& hourAngle, double& declination);
    int     getCurrentDay() { return m_currentDay; }

    struct SSRTsolarDataRecord{
    double	culmination;
    double	declination;
    double	radius;
    double	P;
    double	B;
    double	L;
    double	dDdt;
    double	d2Ddt2;
    double	dHdt;
    double	d2Hdt2;
    void	fromString(QString&,QSoldatFile*);
    };
    struct SSRTsolarDataRecord* getCurrentRecord() {return m_pRecords + m_currentDay; }

private:
	QString m_solarDataTitleString;
    struct SSRTsolarDataRecord* m_pRecords;

	int m_recordsAmount;
	int m_currentDay;
	double m_c;
	double m_n;
	double m_base;
	double m_pi;
	double m_phi;
	double m_w;
	double m_time;
	double m_frequency;
	QDate  m_date;
	int    m_currentNorthSouthOrder;
	int    m_currentEastWestOrder;
	int    calcNorthSouthOrder();
	int    calcEastWestOrder();
};
//}

#endif
