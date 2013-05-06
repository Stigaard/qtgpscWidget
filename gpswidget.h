/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2013  <copyright holder> <email>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


#ifndef GPSWIDGET_H
#define GPSWIDGET_H

#include "satview.h"
#include "satellite.h"
#include "../gpsReader/gpsreader.h"

#include <QWidget>
#include <QButtonGroup>
#include <QRadioButton>
#include <QGridLayout>
#include <QSettings>


class gpsWidget : public QWidget
{
  Q_OBJECT
public:
    gpsWidget(gpsReader * gps, QWidget* parent = 0);
    
private:
    void loadGPS(void);
    gpsReader * gps;
    SatView *satWidget;
    QButtonGroup *gpsQuality;
    QRadioButton *gpsQualityInvalid, *gpsQualityGPSFix, *gpsQualityDGPSFix, *gpsQualityPPSFix,
                 *gpsQualityRTKFix, *gpsQualityFRTKFix, *gpsQualityEstimated, *gpsQualityManual,
                 *gpsQualitySimulation;
    QGridLayout *gpsLayout;
    void drawGPSGui(void);
    
private slots:
    void updateSatlist(SatList sats);
    void updateSatStatus(QByteArray time, QByteArray latitude, char latitudeHeading, 
              QByteArray longitude, char longitudeHeading, int GPSQuality, 
              int sattelitesInView, float horizontalDilution, float altitude,
              char altitudeUnit, QByteArray geoidalSeperation, char geoidalSeperationUnit,
              float dGPSAge, int dGPSStation);
    void VTGReceiver(QByteArray trackMadeGood, QByteArray trackMadeGoodIndicator,
	      QByteArray MagneticTrackMadeGood, QByteArray MagneticTrackMadeGoodIndicator,
	      QByteArray GroundSpeedInKnots, QByteArray GroundSpeedInKnotsUnit,
	      float GroundSpeedInKmh, QByteArray GroundSpeedInKmhUnit);
signals:
    void velocity(float kmh);
};

#endif // GPSWIDGET_H
