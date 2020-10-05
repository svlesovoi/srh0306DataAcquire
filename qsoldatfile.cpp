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
#include "qsoldatfile.h"
#include <qmessagebox.h>
#include <math.h>
#include <stdlib.h>
#include <qtextstream.h>

//namespace std {
/*
QMessageBox::information(NULL,"",solDayText.mid(12,11));
QMessageBox::information(NULL,"",solDayText.mid(23,10));
QMessageBox::information(NULL,"",solDayText.mid(33,6));
QMessageBox::information(NULL,"",solDayText.mid(39,5));
QMessageBox::information(NULL,"",solDayText.mid(44,5));
QMessageBox::information(NULL,"",solDayText.mid(49,6));
QMessageBox::information(NULL,"",solDayText.mid(55,6));
*/
void QSoldatFile::SSRTsolarDataRecord::fromString(QString& recordString, QSoldatFile* pOwner){
	QString aux;
	aux = recordString.mid(12,11);
	declination =	aux.mid(1,2).toDouble() * 3600.	+ 
			aux.mid(4,2).toDouble() * 60.	+
			aux.mid(7,4).toDouble();
	declination *= aux[0] == '-' ? -1. : 1.;
	declination  = pOwner->arcsecToRad(declination);

	aux = recordString.mid(23,10);
	culmination =	aux.mid(0,2).toDouble() * 3600.	+ 
			aux.mid(3,2).toDouble() * 60.	+
			aux.mid(6,4).toDouble();

	aux = recordString.mid(33,6);
	radius = aux.toDouble();
	radius = pOwner->arcminToRad(radius);
	
	aux = recordString.mid(39,5);
	P = aux.toDouble();
	P = pOwner->degToRad(P);

	aux = recordString.mid(44,5);
	B = aux.toDouble();
	B = pOwner->degToRad(B);

	aux = recordString.mid(49,6);
	L = aux.toDouble();
	L = pOwner->degToRad(L);

	aux = recordString.mid(55,6);
	dDdt = aux.toDouble();
	dDdt = pOwner->arcsecToRad(dDdt) / 3600.;

	d2Ddt2	= 0.;
	dHdt	= 0.;
	d2Hdt2	= 0.;
}

QSoldatFile::QSoldatFile()
: QFile(),
m_currentDay(0),
m_n(128.),
m_pi(3.14159265358979323846),	
m_phi(0.903338787600965),			//[rad]
m_c(2.99793e8),					//[m/s]
m_base(4.9),					//[m]
m_frequency(5730000000.),			//[Hz]
m_w(15.*3.14159265358979323846/(180.*3600.))	//[rad/s]
{
	m_pRecords = 0;
}

QSoldatFile::~QSoldatFile(){
	if (m_pRecords)
		delete [] m_pRecords;
}

bool QSoldatFile::fromFile(QString& soldatName){
	setFileName(soldatName);
	if (open(QIODevice::ReadOnly)){
		QTextStream soldatText(this);
		QString title = soldatText.readLine();
		int headerSize = title.length() + 2;
		QString delim = soldatText.readLine();
		headerSize += delim.length() + 2;
		QString fields = soldatText.readLine();
		headerSize += fields.length() + 2;
		QString dims = soldatText.readLine();
		headerSize += dims.length() + 2;
		delim = soldatText.readLine();
		headerSize += delim.length() + 2;
		QString solDayText = soldatText.readLine();
		int solDayTextLength = solDayText.length() + 2;
		m_recordsAmount = (size() - headerSize) / solDayTextLength;
		m_pRecords = new SSRTsolarDataRecord[m_recordsAmount];
		solDayText.replace(QChar(solDayText[0]),"|");
		int i = 0;
		m_pRecords[0].fromString(solDayText,this);
		for (int i = 1;i < m_recordsAmount;++i) { 
			solDayText = soldatText.readLine();
			m_pRecords[i].fromString(solDayText,this);
/*
if (i == 315){
QMessageBox::information(NULL,"",solDayText);
QString msg;
msg.sprintf("%f",m_pRecords[i].culmination);
QMessageBox::information(NULL,"",msg);
}
*/
		}
		close();
		double	T2	= 48.*3600.;
		double	H2	= 4. * m_pi;
		for (i = 1;i < m_recordsAmount - 1;++i){
			double	dD2		= (m_pRecords[i + 1].dDdt - m_pRecords[i - 1].dDdt);
			m_pRecords[i].d2Ddt2	= dD2 / (T2 + m_pRecords[i + 1].culmination - m_pRecords[i - 1].culmination);
			m_pRecords[i].dHdt	= H2  / (T2 + m_pRecords[i + 1].culmination - m_pRecords[i - 1].culmination);
		}

		for (i = 1;i < m_recordsAmount - 1;++i){
			double dH2		= (m_pRecords[i + 1].dHdt - m_pRecords[i - 1].dHdt);
			m_pRecords[i].d2Hdt2	= dH2 / (T2 + m_pRecords[i + 1].culmination - m_pRecords[i - 1].culmination);
		}
		return true;
	} else {
		QMessageBox::information(NULL,"QSoldatFile","Unable to open soldat.txt","");
		return false;
	}
}

double QSoldatFile::radToDeg(double rad){
	return rad / QSoldatFile::m_pi * 180.;
}

double QSoldatFile::degToRad(double deg){
	return deg / 180. * m_pi;
}

double QSoldatFile::radToArcsec(double rad){
	return rad / m_pi * (180. * 3600.);
}

double QSoldatFile::arcsecToRad(double arcsec){
	return arcsec / (180. * 3600.) * m_pi;
}

double QSoldatFile::radToArcmin(double rad){
	return rad / m_pi * (180. * 60.);
}

double QSoldatFile::arcminToRad(double arcsec){
	return arcsec / (180. * 60.) * m_pi;
}

bool QSoldatFile::IsValidCurrentDay(){
	return m_currentDay >= 0 && m_currentDay < m_recordsAmount;
}

bool QSoldatFile::IsValidTime(){
	return m_time > -24.*3600 && m_time < 24.*3600.;
}

int QSoldatFile::calcEastWestOrder(){
	double cosP = sin(TDHourAngle())*cos(TDDeclination());
	double dOrder = cosP * m_base * m_frequency / m_c;
	return dOrder < 0. ? (short)(dOrder - .5) : (short)(dOrder + .5);
}

int QSoldatFile::calcNorthSouthOrder(){
	double H = TDHourAngle();
	double D = TDDeclination();
	double cosQ = cos(H)*cos(D)*sin(m_phi) - sin(D)*cos(m_phi);
	double dOrder = cosQ * m_base * m_frequency / m_c;
	return dOrder < 0. ? (short)(dOrder - .5) : (short)(dOrder + .5);
}

int QSoldatFile::EastWestOrder(double t){
	if (t != -10.e6){
		double storeT = m_time;
		m_time = t;
		int result = calcEastWestOrder();
		m_time = storeT;
		return result;
	}
	return calcEastWestOrder();
}

int QSoldatFile::NorthSouthOrder(double t){
	if (t != -10.e6){
		double storeT = m_time;
		m_time = t;
		int result = calcNorthSouthOrder();
		m_time = storeT;
		return result;
	}
	return calcNorthSouthOrder();
}

QString QSoldatFile::Culmination(){
	QString strResult;
	if (m_recordsAmount){
		double sec = m_pRecords[m_currentDay].culmination;
		int hour = (int)(sec / 3600.);	sec -= hour * 3600.;
		int min  = (int)(sec / 60.);	sec -= min * 60.;
		int iSec = (int)sec;
		int rSec = (int)((sec - iSec) * 1000.);
		strResult.sprintf("%02u:%02u:%02u.%03u",hour,min,iSec,rSec);
	}
	else	
		strResult.sprintf("NotSolarData");
	return strResult;
}

QString QSoldatFile::Declination(){
	QString strResult;
	if (m_recordsAmount){
		double sec = radToArcsec(fabs(m_pRecords[m_currentDay].declination));
		int deg = (int)(sec / 3600.);	sec -= deg * 3600.;
		int min = (int)(sec / 60.);	sec -= min * 60.;
		int iSec = (int)sec;
		int rSec = (int)((sec - iSec) * 1000.);
		strResult.sprintf("%s%02u:%02u:%02u.%03u",
		m_pRecords[m_currentDay].declination < 0. ? "-":" ",deg,min,iSec,rSec);
	}
	else	
		strResult.sprintf("NotSolarData");

	return strResult;
}

QString QSoldatFile::Radius(){
	QString strResult;
	if (m_recordsAmount)
		strResult.sprintf("%f",radToArcmin(m_pRecords[m_currentDay].radius));
	else	
		strResult.sprintf("NotSolarData");
	return strResult;
}

QString QSoldatFile::PAngle(){
	QString strResult;
	if (m_recordsAmount)
		strResult.sprintf("%f",radToDeg(m_pRecords[m_currentDay].P));
	else	
		strResult.sprintf("NotSolarData");
	return strResult;
}

QString QSoldatFile::BAngle(){
	QString strResult;
	if (m_recordsAmount)
		strResult.sprintf("%f",radToDeg(m_pRecords[m_currentDay].B));
	else	
		strResult.sprintf("NotSolarData");
	return strResult;
}

QString QSoldatFile::LAngle(){
	QString strResult;
	if (m_recordsAmount)
		strResult.sprintf("%f",radToDeg(m_pRecords[m_currentDay].L));
	else	
		strResult.sprintf("NotSolarData");
	return strResult;
}

QString QSoldatFile::DDdt(){
	QString strResult;
	if (m_recordsAmount)
		strResult.sprintf("%f",radToArcsec(m_pRecords[m_currentDay].dDdt));
	else	
		strResult.sprintf("NotSolarData");

	return strResult;
}

double QSoldatFile::DRadius(){
	return IsValidCurrentDay() ? m_pRecords[m_currentDay].radius : 0.0;
}

double QSoldatFile::DCulmination(){
	return IsValidCurrentDay() ? m_pRecords[m_currentDay].culmination : 0.0;
}

double QSoldatFile::DDeclination(){
	return IsValidCurrentDay() ? m_pRecords[m_currentDay].declination : 0.0;
}

double QSoldatFile::DPAngle(){
	return IsValidCurrentDay() ? m_pRecords[m_currentDay].P : 0.0;
}

double QSoldatFile::DBAngle(){
	return IsValidCurrentDay() ? m_pRecords[m_currentDay].B : 0.0;
}

double QSoldatFile::DLAngle(){
	return IsValidCurrentDay() ? m_pRecords[m_currentDay].L : 0.0;
}

double QSoldatFile::DDDdt(){
	return IsValidCurrentDay() ? m_pRecords[m_currentDay].dDdt : 0.0;
}

double QSoldatFile::TDDeclination(){
	SSRTsolarDataRecord& data = m_pRecords[m_currentDay];
	double	t	= m_time - m_pRecords[m_currentDay].culmination;
	return	data.declination + data.dDdt * t + data.d2Ddt2/2. * t*t;
}

double QSoldatFile::TDHourAngle(){
	SSRTsolarDataRecord& data = m_pRecords[m_currentDay];
	double	t	= m_time - m_pRecords[m_currentDay].culmination;
	return	data.dHdt * t + data.d2Hdt2/2. * t*t;
}

int QSoldatFile::TEastWestOrder(double frequency){
	double f = frequency == 0. ? m_frequency : frequency;
	double cosP = sin(TDHourAngle())*cos(TDDeclination());
	double dOrder = cosP * m_base * f / m_c;
	return dOrder < 0. ? (int)(dOrder - .5) : (int)(dOrder + .5);
}

int QSoldatFile::TNorthSouthOrder(double frequency){
	double f = frequency == 0. ? m_frequency : frequency;
	double cosQ = cos(TDHourAngle())*cos(TDDeclination())*sin(m_phi) - sin(TDDeclination())*cos(m_phi);
	double dOrder = cosQ * m_base * f / m_c;
	return dOrder < 0. ? (int)(dOrder - .5) : (int)(dOrder + .5);
}

double QSoldatFile::OP(double frequency){
	double arg = m_currentEastWestOrder * m_c / (m_base * frequency);
	if (fabs(arg) <= 1.)	return acos(arg);
	if (arg < 0.)		return m_pi;
				return 0.;
}

double QSoldatFile::OQ(double frequency){
	double arg = m_currentNorthSouthOrder * m_c / (m_base * frequency);
	if (fabs(arg) <= 1.)	return acos(arg);
	if (arg < 0.)		return m_pi;
				return 0.;
}

double QSoldatFile::TP(){
	double arg = sin(TDHourAngle()) * cos(TDDeclination());
	if (fabs(arg) <= 1.)	return acos(arg);
	if (arg < 0.)		return m_pi;
				return 0.;
}

double QSoldatFile::TQ(){
	double arg = cos(TDHourAngle())*cos(TDDeclination())*sin(m_phi) - sin(TDDeclination())*cos(m_phi);
	if (fabs(arg) <= 1.)	return acos(arg);
	if (arg < 0.)		return m_pi;
				return 0.;
}

double QSoldatFile::TDEastWestBeamAngle(){
	return atan( tan(TDHourAngle()) * sin(TDDeclination()) );
}

double QSoldatFile::TDNorthSouthBeamAngle(){
	double h = TDHourAngle();
	if (fabs(h) < 0.0001){
		if (h < 0.) h = -0.0001;
		else        h =  0.0001;
	}
	return atan( -( sin(TDDeclination()) / tan(h) + cos(TDDeclination()) / (sin(h) * tan(m_phi)) ));
}

double QSoldatFile::TDEastWestBeamWidth(){
	double h = TDHourAngle();
	double _cosP = sin(h) * cos(TDDeclination());
	return 0.886 * (1./m_n) * m_c/(m_frequency * m_base) * (1./sqrt(1. - _cosP * _cosP));
}

double QSoldatFile::TDNorthSouthBeamWidth(){
	double h = TDHourAngle();
	double _cosQ = cos(h) * cos(TDDeclination()) * sin(m_phi) - sin(TDDeclination()) * cos(m_phi);
	return 0.886 * (1./m_n) * m_c/(m_frequency * m_base) * (1./sqrt(1. - _cosQ * _cosQ));
}

QString QSoldatFile::EarthRotationPeriod(){
	QString strResult;
	double sec = DEarthRotationPeriod();
	int hour = (int)(sec / 3600.);	sec -= hour * 3600.;
	int min  = (int)(sec / 60.);	sec -= min * 60.;
	int iSec = (int)sec;
	int rSec = (int)((sec - iSec) * 1000.);
	strResult.sprintf("%02u:%02u:%02u.%03u",hour,min,iSec,rSec);
	return strResult;
}

double QSoldatFile::DEarthRotationPeriod(){
	double prevNoon = m_currentDay > 0	
		? m_pRecords[m_currentDay - 1].culmination
		: 6.*3600.;
	double nextNoon = m_currentDay < m_recordsAmount - 1
		? m_pRecords[m_currentDay + 1].culmination
		: 6.*3600.;
	return (nextNoon - prevNoon)*.5 + 24.*3600.;
}

double QSoldatFile::DDHdt(){
	return IsValidCurrentDay() ? m_pRecords[m_currentDay].dHdt : 0.0;
}

double QSoldatFile::DD2Ddt2(){
	return IsValidCurrentDay() ? m_pRecords[m_currentDay].d2Ddt2 : 0.0;
}

double QSoldatFile::DD2Hdt2(){
	return IsValidCurrentDay() ? m_pRecords[m_currentDay].d2Hdt2 : 0.0;
}

QString QSoldatFile::getSolarDataTitle(){
	QString strResult = m_solarDataTitleString;
	return strResult;
}

void QSoldatFile::setSolarDataTitle(const char* pTitle){
	m_solarDataTitleString = QString(pTitle);
}

int QSoldatFile::getCurrentEastWestOrder(){
	return m_currentEastWestOrder;
}

void QSoldatFile::setCurrentEastWestOrder(int ewOrder){
	m_currentEastWestOrder = ewOrder;
}

int QSoldatFile::getCurrentNorthSouthOrder(){
	return m_currentNorthSouthOrder;
}

void QSoldatFile::setCurrentNorthSouthOrder(int nsOrder){
	m_currentNorthSouthOrder = nsOrder;
}

bool QSoldatFile::setDate(QDate& theDate){
	m_date = theDate;
	QDate firstDate(1991,12,31);
	m_currentDay = firstDate.daysTo(m_date);
	return IsValidCurrentDay();
}

bool QSoldatFile::setTime(double theT){
	m_time = theT;
//	const double timeLimit = 22.*3600.;
//	if (m_time > timeLimit){
//		m_time = timeLimit - m_time;
//		m_date = m_date.addDays(1);
//	}
	return IsValidTime();
}

double  QSoldatFile::delayForPair(int A1, int A2){
	if (A1 > 0 && A1 <= m_n && A2 > m_n && A2 <= 2*m_n){
		double H = TDHourAngle();
		double D = TDDeclination();
		double cosP = sin(H)*cos(D);
		double cosQ = cos(H)*cos(D)*sin(m_phi) - sin(D)*cos(m_phi);
		double N = (m_n/2. + .5 - A1);
		double M = (m_n/2. + .5 - (A2 - m_n));
		return m_base*(N*cosP - M*cosQ) / m_c;
	}
	return 0.;
}

double  QSoldatFile::delayForEwPair(int A1, int A2){
	if (A1 > 0 && A1 <= m_n && A2 > 0 && A2 <= m_n && A1 != A2){
		double cosP = sin(TDHourAngle()) * cos(TDDeclination());
		double N	= A1 - A2;
		return m_base*N*cosP / m_c;
	}
	return 0.;
}

double  QSoldatFile::delayForNsPair(int A1, int A2){
	if (A1 > m_n && A1 <= 2*m_n && A2 > m_n && A2 <= 2*m_n && A1 != A2){
		double cosQ = cos(TDHourAngle())*cos(TDDeclination())*sin(m_phi) - sin(TDDeclination())*cos(m_phi);
		double M = A1 - A2;
		return m_base*M*cosQ / m_c;
	}
	return 0.;
}

void	QSoldatFile::hor2eq(double altitude, double azimuth, double& hourAngle, double& declination){
	double sinDe = sin(m_phi)*sin(altitude) - cos(m_phi)*cos(altitude)*cos(azimuth);
	double sinHa = sin(azimuth)*cos(altitude)/sqrt(1. - sinDe*sinDe);
	
	declination = asin(sinDe);
	hourAngle = asin(sinHa);
}

double  QSoldatFile::delayForAntenna(int A, double hourAngle, double declination){
	if (A > 0 && A <= m_n){
		double cosP = sin(hourAngle)*cos(declination);
		double N = 63.5 - A;
		return m_base*N*cosP / m_c;
	} else if (A > m_n && A <= 2*m_n){
		double cosQ = cos(hourAngle)*cos(declination)*sin(m_phi) - sin(declination)*cos(m_phi);
		double M = 191.5 - A;
		return m_base*M*cosQ / m_c;
	}
	return 0.;
}

bool  	QSoldatFile::uvwForPair(int A1, int A2, double& u, double& v, double& w){
	double X1, Y1;
	double X2, Y2;
	double X, Y, Z;
	double XX, YY, ZZ;
	double H, D;
	if (A1 >= 1 && A1 <= 2*m_n && A2 >=1 && A2 <= 2*m_n){
		if (A1 <= m_n){					//East-West {1..128}
			Y1 = (m_n/2. + .5 - A1);
			X1 = 0.;
		} else {					//North-South {129..256}
			Y1 = 0.;
			X1 = (m_n/2. + .5 - (A1 - m_n));
		}
		if (A2 <= m_n){					//East-West {1..128}
			Y2 = (m_n/2. + .5 - A2);
			X2 = 0.;
		} else {					//North-South {129..256}
			Y2 = 0.;
			X2 = (m_n/2. + .5 - (A2 - m_n));
		}
		X = (X2 - X1)*m_base;
		Y = (Y2 - Y1)*m_base;
		Z = 0.;
		XX = cos(m_phi)*X - sin(m_phi)*Z;
		YY = Y;
		ZZ = sin(m_phi)*X + cos(m_phi)*Z;

		H = TDHourAngle();
		D = TDDeclination();
		u =         sin(H)*XX +        cos(H)*YY;
		v = -sin(D)*cos(H)*XX + sin(D)*sin(H)*YY + cos(D)*ZZ;
		w =  cos(D)*cos(H)*XX - cos(D)*sin(H)*YY + sin(D)*ZZ;

		return true;
	}
	return false;
}
