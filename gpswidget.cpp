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


#include "gpswidget.h"
#include "gpswidget.moc"

gpsWidget::gpsWidget(gpsReader * gps, QWidget* parent)
{
  this->gps = gps;
  this->loadGPS();
  this->drawGPSGui();
//  this->Layout->addWidget(gpsWidget, 1,3);
}

void gpsWidget::loadGPS(void )
{
    connect(this->gps, SIGNAL(satellitesUpdated(SatList)), this, SLOT(updateSatlist(SatList)));
    connect(this->gps, SIGNAL(newGGA(QByteArray,QByteArray,char,QByteArray,char,int,int,float,float,char,QByteArray,char,float,int)),
    this, SLOT(updateSatStatus(QByteArray,QByteArray,char,QByteArray,char,int,int,float,float,char,QByteArray,char,float,int)));
    connect(this->gps, SIGNAL(newVTG(QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,float,QByteArray)),
	    this, SLOT(VTGReceiver(QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,QByteArray,float,QByteArray)));
    //connect(this, SIGNAL(velocity(float)), this->modi, SLOT(velocityReceiver(float)));
}

void gpsWidget::drawGPSGui(void )
{
    //this->gpsWidget = new QWidget(this->globalWidget);
    this->gpsLayout = new QGridLayout;
    this->satWidget = new SatView();
    
    this->gpsQuality = new QButtonGroup(this);
    this->gpsQualityInvalid = new QRadioButton("Invalid");
    this->gpsQualityGPSFix = new QRadioButton("GPS Fix");
    this->gpsQualityDGPSFix = new QRadioButton("DGPS fix");
    this->gpsQualityPPSFix = new QRadioButton("PPS fix");
    this->gpsQualityRTKFix = new QRadioButton("RTK fix");
    this->gpsQualityFRTKFix = new QRadioButton("Float RTK");
    this->gpsQualityEstimated = new QRadioButton("Dead reckoning");
    this->gpsQualityManual = new QRadioButton("Manual input");
    this->gpsQualitySimulation = new QRadioButton("Simulation");
    this->gpsQuality->addButton(this->gpsQualityInvalid);
    this->gpsQuality->addButton(this->gpsQualityGPSFix);
    this->gpsQuality->addButton(this->gpsQualityDGPSFix);
    this->gpsQuality->addButton(this->gpsQualityPPSFix);
    this->gpsQuality->addButton(this->gpsQualityRTKFix);
    this->gpsQuality->addButton(this->gpsQualityFRTKFix);
    this->gpsQuality->addButton(this->gpsQualityEstimated);
    this->gpsQuality->addButton(this->gpsQualityManual);
    this->gpsQuality->addButton(this->gpsQualitySimulation);
    this->gpsLayout->addWidget(this->satWidget,0,0);
    this->gpsLayout->addWidget(this->gpsQualityInvalid, 1,0);
    this->gpsLayout->addWidget(this->gpsQualityGPSFix, 2,0);
    this->gpsLayout->addWidget(this->gpsQualityDGPSFix, 3,0);
    this->gpsLayout->addWidget(this->gpsQualityPPSFix, 4,0);
    this->gpsLayout->addWidget(this->gpsQualityRTKFix,5,0);
    this->gpsLayout->addWidget(this->gpsQualityFRTKFix, 6,0);
    this->gpsLayout->addWidget(this->gpsQualityEstimated, 7,0);
    this->gpsLayout->addWidget(this->gpsQualityManual, 8,0);
    this->gpsLayout->addWidget(this->gpsQualitySimulation, 9,0);
    this->setLayout(this->gpsLayout);
    
    this->gpsQualityInvalid->setChecked(true);
}

void gpsWidget::updateSatlist(SatList sats)
{
  SatList seenPRNs;
  for(int i=0;i<sats.count();i++)
  {
    if(sats[i].prn != -1)
      seenPRNs.push_back(sats[i]);
  }
  this->satWidget->setSatellites(seenPRNs);
}

void gpsWidget::updateSatStatus(QByteArray time, QByteArray latitude, char latitudeHeading, QByteArray longitude, char longitudeHeading, int GPSQuality, int sattelitesInView, float horizontalDilution, float altitude, char altitudeUnit, QByteArray geoidalSeperation, char geoidalSeperationUnit, float dGPSAge, int dGPSStation)
{
  switch(GPSQuality)
  {
    case 0:
//       this->setStyleSheet("{background-color: red; }");
      this->gpsQualityInvalid->setChecked(true);
      break;
    case 1:
//       this->setStyleSheet("{background-color: yellow; }");
      this->gpsQualityGPSFix->setChecked(true);
      break;
    case 2:
//       this->setStyleSheet("{background-color: yellow; }");
      this->gpsQualityDGPSFix->setChecked(true);
      break;
    case 3:
      this->gpsQualityPPSFix->setChecked(true);
      break;
    case 4:
//       this->setStyleSheet("{background-color: green; }");
      this->gpsQualityRTKFix->setChecked(true);
      break;
    case 5:
//       this->setStyleSheet("{background-color: green; }");
      this->gpsQualityFRTKFix->setChecked(true);
      break;
    case 6:
      this->gpsQualityEstimated->setChecked(true);
      break;
    case 7:
      this->gpsQualityManual->setChecked(true);
      break;
    case 8:
      this->gpsQualitySimulation->setChecked(true);
      break;
    default:
      this->gpsQualityInvalid->setChecked(true);
      break;
    
  }
}

void gpsWidget::VTGReceiver(QByteArray trackMadeGood, QByteArray trackMadeGoodIndicator, QByteArray MagneticTrackMadeGood, QByteArray MagneticTrackMadeGoodIndicator, QByteArray GroundSpeedInKnots, QByteArray GroundSpeedInKnotsUnit, float GroundSpeedInKmh, QByteArray GroundSpeedInKmhUnit)
{
  emit(velocity(GroundSpeedInKmh));
}