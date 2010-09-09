/**************************************************************************
*   Copyright (C) 2010 by Diego Berge <gpsd@navlost.eu>                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 3 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, see <http://www.gnu.org/licenses/>   *
***************************************************************************/

#include <math.h>
#include <QDateTime>
#include <QMessageBox>
#include <QMimeData>
#include <QClipboard>
#include <QSettings>
#include "gps.h"
#include "qtgpsc.h"

QtGpsc::QtGpsc(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
    // Create action groups
    QActionGroup *altitudeGroup = new QActionGroup(this);
    altitudeGroup->setExclusive(true);
    altitudeGroup->addAction(ui.actionMetres);
    altitudeGroup->addAction(ui.actionFeet);
    
    QActionGroup *distanceGroup = new QActionGroup(this);
    distanceGroup->setExclusive(true);
    distanceGroup->addAction(ui.actionKilometres);
    distanceGroup->addAction(ui.actionStatute_Miles);
    distanceGroup->addAction(ui.actionNautical_Miles);
    
    QActionGroup *speedGroup = new QActionGroup(this);
    speedGroup->setExclusive(true);
    speedGroup->addAction(ui.actionMetres_per_second);
    speedGroup->addAction(ui.actionKilometres_per_hour);
    speedGroup->addAction(ui.actionMiles_per_hour);
    speedGroup->addAction(ui.actionKnots);
    
    QActionGroup *positionGroup = new QActionGroup(this);
    positionGroup->setExclusive(true);
    positionGroup->addAction(ui.actionDegrees_minutes_seconds);
    positionGroup->addAction(ui.actionDegrees_minutes);
    positionGroup->addAction(ui.actionDegrees);
    positionGroup->addAction(ui.actionGrads);
    positionGroup->addAction(ui.actionRadians);
    
    readSettings();
    
    // Connect signals
    connect(ui.connectButton, SIGNAL(toggled(bool)), this, SLOT(handleConnection(bool)));
    connect(&gpsd, SIGNAL(connectionStatus(bool)), ui.connectButton, SLOT(setChecked(bool)));
    
    connect(&gpsd, SIGNAL(positionUpdated()), this, SLOT(updatePosition()));
    connect(&gpsd, SIGNAL(constellationUpdated()), this, SLOT(updateConstellation()));
    connect(&gpsd, SIGNAL(deviceListUpdated()), this, SLOT(updateDeviceList()));
    connect(&gpsd, SIGNAL(dataUpdated()), this, SLOT(updateData()));
    connect(&gpsd, SIGNAL(connectionError(int)), this, SLOT(handleError(int)));
    
    connect(ui.actionMetres, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionFeet, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionKilometres, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionStatute_Miles, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionNautical_Miles, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionMetres_per_second, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionKilometres_per_hour, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionMiles_per_hour, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionKnots, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionDegrees_minutes_seconds, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionDegrees_minutes, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionDegrees, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionGrads, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    connect(ui.actionRadians, SIGNAL(toggled(bool)), this, SLOT(unitsChanged(bool)));
    
    connect(this, SIGNAL(settingsChanged()), this, SLOT(writeSettings()));
    
    connect(ui.actionCopy_Position_as_KML, SIGNAL(triggered()), this, SLOT(copyAsKml()));
    connect(ui.actionCopy_Position_as_CSV, SIGNAL(triggered()), this, SLOT(copyAsCsv()));
    
    connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(aboutBox()));
    
}

void QtGpsc::writeSettings()
{
    QSettings settings("GPSD", "qtgpsc");
    settings.setValue("/gpsd/qtgpsc/units/position", latLonUnits);
    settings.setValue("/gpsd/qtgpsc/units/distance", distanceUnits);
    settings.setValue("/gpsd/qtgpsc/units/speed", speedUnits);
    settings.setValue("/gpsd/qtgpsc/units/altitude", altitudeUnits);
    settings.setValue("/gpsd/qtgpsc/host", ui.host->text());
    settings.setValue("/gpsd/qtgpsc/port", ui.port->value());
}

void QtGpsc::readSettings()
{
    bool signalsBlocked = this->blockSignals(true);
    
    QSettings settings("GPSD", "qtgpsc");
    latLonUnits = (Gpsd::LatLonUnit)settings.value("/gpsd/qtgpsc/units/position").toInt();
    distanceUnits = (Gpsd::DistanceUnit)settings.value("/gpsd/qtgpsc/units/distance").toInt();
    speedUnits = (Gpsd::SpeedUnit)settings.value("/gpsd/qtgpsc/units/speed").toInt();
    altitudeUnits = (Gpsd::AltitudeUnit)settings.value("/gpsd/qtgpsc/units/altitude").toInt();
    ui.host->setText(settings.value("/gpsd/qtgpsc/host", "localhost").toString());
    ui.port->setValue(settings.value("/gpsd/qtgpsc/port", "2947").toInt());
    ui.rawDataViewer->setHost(ui.host->text());
    ui.rawDataViewer->setPort(ui.port->value());
    
    switch (latLonUnits) {
        case Gpsd::DegreesMinutesSeconds:
            ui.actionDegrees_minutes_seconds->setChecked(true);
            break;
        case Gpsd::DegreesMinutes:
            ui.actionDegrees_minutes->setChecked(true);
            break;
        case Gpsd::Degrees:
            ui.actionDegrees->setChecked(true);
            break;
        case Gpsd::Grads:
            ui.actionGrads->setChecked(true);
            break;
        case Gpsd::Radians:
            ui.actionRadians->setChecked(true);
            break;
    }
    
    switch (distanceUnits) {
        case Gpsd::Kilometre:
            ui.actionKilometres->setChecked(true);
            break;
        case Gpsd::StatuteMile:
            ui.actionStatute_Miles->setChecked(true);
            break;
        case Gpsd::NauticalMile:
            ui.actionNautical_Miles->setChecked(true);
            break;
    }
    
    switch (speedUnits) {
        case Gpsd::MPS:
            ui.actionMetres_per_second->setChecked(true);
            break;
        case Gpsd::KPH:
            ui.actionKilometres_per_hour->setChecked(true);
            break;
        case Gpsd::MPH:
            ui.actionMiles_per_hour->setChecked(true);
            break;
        case Gpsd::Kt:
            ui.actionKnots->setChecked(true);
            break;
    }
    
    switch (altitudeUnits) {
        case Gpsd::Metre:
            ui.actionMetres->setChecked(true);
            break;
        case Gpsd::Foot:
            ui.actionFeet->setChecked(true);
            break;
    }
    
    this->blockSignals(signalsBlocked);
}

void QtGpsc::setHost(const QString &h)
{
    ui.host->setText(h);
}

void QtGpsc::setPort(quint16 p)
{
    ui.port->setValue(p);
}

void QtGpsc::setDevice(const QString &d)
{
    int idx;
    if ((idx = ui.device->findText(d)) != -1) {
        ui.device->setCurrentIndex(idx);
    } else {
        ui.device->addItem(d);
        ui.device->setCurrentIndex(ui.device->count() - 1);
    }
}

void QtGpsc::handleConnection(bool conn)
{
    if (conn) {
        if (!ui.host->text().isEmpty() && ui.port->value() != 0) {
            ui.connectButton->setText("Connecting...");
            gpsd.connectToServer(ui.host->text(), ui.port->value());
            if (gpsd.isConnected()) {
                ui.connectButton->setText("Disc&onnect");
            } else {
               ui. connectButton->setChecked(false);
            }
            emit settingsChanged();
        } else {
            ui.connectButton->setChecked(false);
        }
    } else {
        if (gpsd.isConnected()) {
            gpsd.disconnectFromServer();
        }
        ui.connectButton->setText("Connect");
    }
}

void QtGpsc::updatePosition()
{
    if (gpsd.isConnected()) {
        
        if (gpsd.flag(LATLON_SET)) {
            double latitude = gpsd.gpsData()->fix.latitude;
            double longitude = gpsd.gpsData()->fix.longitude;
            ui.latitude->setText(Gpsd::convertLatLon(true, latitude, latLonUnits));
            ui.longitude->setText(Gpsd::convertLatLon(false, longitude, latLonUnits));
            
//             ui.marbleMap->setHome(longitude, latitude, 1500);
//             ui.marbleMap->centerOn(longitude, latitude, true);
        }
        
        if (gpsd.flag(TIME_SET)) {
            QDateTime t;
            t.setTimeSpec(Qt::UTC);
            t.setTime_t(gpsd.gpsData()->fix.time);
            ui.tstamp->setText(t.toString(Qt::ISODate));
        }
        
        if (gpsd.flag(ALTITUDE_SET)) {
            ui.elevation->setText(QString("%1 %2").arg(Gpsd::convertAltitude(gpsd.gpsData()->fix.altitude, altitudeUnits), 0, 'f', 2)
            .arg(Gpsd::suffix(altitudeUnits)));
        }
        
        if (gpsd.flag(SPEED_SET)) {
            ui.speed->setText(QString("%1 %2").arg(Gpsd::convertSpeed(gpsd.gpsData()->fix.speed, speedUnits), 0, 'f', 0)
            .arg(Gpsd::suffix(speedUnits)));
        }
        
        if (gpsd.flag(TRACK_SET)) {
            ui.track->setText(QString("%1").arg(Gpsd::convertTrack(gpsd.gpsData()->fix.track, latLonUnits)));
        }
        
        if (gpsd.flag(DOP_SET) || true) {
            QString s("P%1 H%2 V%3 T%4 G%5");
            s = s.arg(gpsd.gpsData()->dop.pdop, 0, 'f', 1)
                .arg(gpsd.gpsData()->dop.hdop, 0, 'f', 1)
                .arg(gpsd.gpsData()->dop.vdop, 0, 'f', 1)
                .arg(gpsd.gpsData()->dop.tdop, 0, 'f', 1)
                .arg(gpsd.gpsData()->dop.gdop, 0, 'f', 1);
            
            ui.dops->setText(s);
        }
        
        if (gpsd.flag(HERR_SET|VERR_SET)) {
            double eph = sqrt(pow(gpsd.gpsData()->fix.epx,2) + pow(gpsd.gpsData()->fix.epy,2));
            QString s("H%1 V%2");
            s = s.arg(Gpsd::convertAltitude(eph, altitudeUnits), 0, 'f', 1)
                .arg(Gpsd::convertAltitude(gpsd.gpsData()->fix.epv, altitudeUnits), 0, 'f', 1);
                
            ui.rms->setText(s);
        }
        
    } else {
        ui.latitude->clear();
        ui.longitude->clear();
        ui.tstamp->clear();
    }
}

void QtGpsc::updateConstellation()
{
    SatList satlist;
    QList<int> visible;
    
    for (int n=0; n<gpsd.gpsData()->satellites_used; n++) {
        visible.append(gpsd.gpsData()->used[n]);
    }
    
    for (int n=0; n<gpsd.gpsData()->satellites_visible; n++) {
        satlist.append(Satellite(
            gpsd.gpsData()->PRN[n],
            gpsd.gpsData()->azimuth[n],
            gpsd.gpsData()->elevation[n],
            gpsd.gpsData()->ss[n],
            visible.contains(gpsd.gpsData()->PRN[n]),
            true
        ));
    }
    
    ui.satView->setSatellites(satlist);
    ui.satTable->setSatellites(satlist);
}

void QtGpsc::updateDeviceList()
{
    // Do not emit any signals while we repopulate the list
    bool signalsBlocked = ui.device->blockSignals(true);
    
    while (ui.device->count())
        ui.device->removeItem(0);
    
    for (int i=0; i<gpsd.gpsData()->devices.ndevices; i++) {
        const char *path = gpsd.gpsData()->devices.list[i].path;
        ui.device->addItem(path);
        if (gpsd.getDevice() == path) {
            ui.device->setCurrentIndex(ui.device->count() - 1);
        }
    }

    ui.device->blockSignals(signalsBlocked);
    
    if (gpsd.getDevice().isEmpty()) {
        if (ui.device->count() > 0) {
            ui.device->setCurrentIndex(0);
        }
    } else {
        if (ui.device->currentIndex() == -1) {
            statusBar()->showMessage(QString("Previously selected device %1 has disappeared!").arg(gpsd.getDevice()));
        }
    }
}

void QtGpsc::updateData()
{
    if (gpsd.flag(DEVICEID_SET) || true) {
        QString m;
        switch (gpsd.gpsData()->fix.mode) {
            case MODE_NOT_SEEN:
                m = "No data";
                break;
            case MODE_NO_FIX:
                m = "No fix";
                break;
            case MODE_2D:
                m = "2D fix";
                break;
            case MODE_3D:
                m = "3D fix";
                break;
            default:
                m = "Unknown mode";
        }
        
        QString s = QString("%1 %2 - %3 - %4/%5 sats used").arg(gpsd.gpsData()->dev.driver)
            .arg(gpsd.gpsData()->dev.subtype)
            .arg(m)
            .arg(gpsd.gpsData()->satellites_used)
            .arg(gpsd.gpsData()->satellites_visible);
            
            statusBar()->showMessage(s);
    }
    
    QString t = QString("qtGpsc - %1:%2%3").arg(ui.host->text())
            .arg(ui.port->value())
            .arg(ui.device->currentText());
        
    setWindowTitle(t);
}

void QtGpsc::handleError(int e)
{
    if (e == 0)
        return;
    
    statusBar()->showMessage(QString("GPSD connection error: %1").arg(gps_errstr(e)));
}

void QtGpsc::unitsChanged(bool checked)
{
    QObject *orig = sender();
    
    if (!orig || !checked)
        return;
    
    if (orig == ui.actionMetres)
        altitudeUnits = Gpsd::Metre;
    else if (orig == ui.actionFeet)
        altitudeUnits = Gpsd::Foot;
    else if (orig == ui.actionKilometres)
        distanceUnits = Gpsd::Kilometre;
    else if (orig == ui.actionStatute_Miles)
        distanceUnits = Gpsd::StatuteMile;
    else if (orig == ui.actionNautical_Miles)
        distanceUnits = Gpsd::NauticalMile;
    else if (orig == ui.actionMetres_per_second)
        speedUnits = Gpsd::MPS;
    else if (orig == ui.actionKilometres_per_hour)
        speedUnits = Gpsd::KPH;
    else if (orig == ui.actionMiles_per_hour)
        speedUnits = Gpsd::MPH;
    else if (orig == ui.actionKnots)
        speedUnits = Gpsd::Kt;
    else if (orig == ui.actionDegrees_minutes_seconds)
        latLonUnits = Gpsd::DegreesMinutesSeconds;
    else if (orig == ui.actionDegrees_minutes)
        latLonUnits = Gpsd::DegreesMinutes;
    else if (orig == ui.actionDegrees)
        latLonUnits = Gpsd::Degrees;
    else if (orig == ui.actionGrads)
        latLonUnits = Gpsd::Grads;
    else if (orig == ui.actionRadians)
        latLonUnits = Gpsd::Radians;
    
    emit settingsChanged();
}

void QtGpsc::aboutBox()
{
    QMessageBox::about(this, "About qtGPSc", QString(
    "<p>Client for the gpsd GPS server<br/>\n"
    "&lt;http://gpsd.berlios.de/&gt;<br/>\n"
    " based on the Qt toolkit.</p>\n"
    "<p>Version %1</p>\n"
    "<p>&copy;2010 Diego Berge &lt;<a href='mailto:gpsd@navlost.eu'>gpsd@navlost.eu</a>&gt;</p>").arg(QTGPSC_VERSION));
}

void QtGpsc::copyAsKml()
{
    QString kml1 = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
    "<kml xmlns=\"http://earth.google.com/kml/2.2\">\n"
    "<Document>\n"
    "   <name>GPSD Position</name>\n"
    "   <StyleMap id=\"msn_star\">\n"
    "       <Pair>\n"
    "           <key>normal</key>\n"
    "           <styleUrl>#sn_star</styleUrl>\n"
    "       </Pair>\n"
    "       <Pair>\n"
    "           <key>highlight</key>\n"
    "           <styleUrl>#sh_star</styleUrl>\n"
    "       </Pair>\n"
    "   </StyleMap>\n"
    "   <Style id=\"sn_star\">\n"
    "       <IconStyle>\n"
    "           <color>ff7faaff</color>\n"
    "           <Icon>\n"
    "               <href>http://maps.google.com/mapfiles/kml/shapes/star.png</href>\n"
    "           </Icon>\n"
    "       </IconStyle>\n"
    "       <ListStyle>\n"
    "       </ListStyle>\n"
    "   </Style>\n"
    "   <Style id=\"sh_star\">\n"
    "       <IconStyle>\n"
    "           <color>ff7faaff</color>\n"
    "           <scale>1.18182</scale>\n"
    "           <Icon>\n"
    "               <href>http://maps.google.com/mapfiles/kml/shapes/star.png</href>\n"
    "           </Icon>\n"
    "       </IconStyle>\n"
    "       <ListStyle>\n"
    "       </ListStyle>\n"
    "   </Style>\n"
    "   <Placemark>\n"
    "       <name>%1</name>\n"
    "       <Snippet>%2</Snippet>\n"
    "       <description><![CDATA[\n"
    "<table>\n"
    "<tr><th>Latitude</th><td>%3</td></tr>\n"
    "<tr><th>Longitude</th><td>%4</td></tr>\n"
    "<tr><th>Altitude</th><td>%5</td></tr>\n"
    "<tr><th>Time</th><td>%6</td></tr>\n"
    "<tr><th>Speed</th><td>%7</td></tr>\n"
    "<tr><th>Track</th><td>%8</td></tr>\n"
    "<tr><th>DOP</th><td>%9</td></tr>\n";
    QString kml2 = "<tr><th>RMS</th><td>%1</td></tr>\n"
    "</table>\n"
    "]]></description>\n"
    "       <styleUrl>#msn_star</styleUrl>\n"
    "       <Point>\n"
    "           <coordinates>%2,%3,%4</coordinates>\n"
    "       </Point>\n"
    "   </Placemark>\n"
    "</Document>\n"
    "</kml>\n";
    
    QString name = QString("%1:%2%3").arg(ui.host->text())
    .arg(ui.port->text())
    .arg(ui.device->currentText());
    
    QString kml = kml1.arg(name)
    .arg(ui.tstamp->text())
    .arg(ui.latitude->text())
    .arg(ui.longitude->text())
    .arg(ui.elevation->text())
    .arg(ui.tstamp->text())
    .arg(ui.speed->text())
    .arg(ui.track->text())
    .arg(ui.dops->text());
    kml += kml2.arg(ui.rms->text())
    .arg(gpsd.gpsData()->fix.longitude)
    .arg(gpsd.gpsData()->fix.latitude)
    .arg(gpsd.gpsData()->fix.altitude);
    
    QApplication::clipboard()->setText(kml);
}

void QtGpsc::copyAsCsv()
{
    QStringList text;
    text << ui.latitude->text() << ui.longitude->text() << ui.elevation->text()
    << ui.tstamp->text() << ui.speed->text() << ui.track->text() 
    << ui.dops->text() << ui.rms->text();
    QApplication::clipboard()->setText(text.join(","));
}
