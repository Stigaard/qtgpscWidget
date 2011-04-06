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

#include <errno.h>
#include <math.h>
#include <QString>
#include "gpsd.h"

Gpsd::Gpsd(QObject *parent)
    : QObject(parent)
{
    host = QString();
    port = 0;
    device = QString();
    // NOTE - I'm not entirely sure if this should be our job
    // or gpsd's to initialise the data structures. I figure
    // since we own this gps_data_t, we might as well do it
    // ourselves.
    gpsdata.gps_fd = 0;
    gpsdata.dev.path[0] = '\0';
    gpsdata.dev.flags = 0;
    gpsdata.dev.driver[0] = '\0';
    gpsdata.dev.subtype[0] = '\0';
    gpsdata.dev.activated = 0;
    gpsdata.dev.baudrate = 0;
    gpsdata.dev.stopbits = 0;
    gpsdata.dev.parity = '\0';
    gpsdata.dev.cycle = 0;
    gpsdata.dev.mincycle = 0;
    gpsdata.dev.driver_mode  = 0;
    
    gpsdata_ext = gpsdata;
}

Gpsd::~Gpsd()
{
    disconnectFromServer();
}

void Gpsd::connectToServer(const QString &h, quint16 p, Gpsd::StreamMode m)
{
    if (isConnected()) {          // We're connected
        disconnectFromServer();   // disconnect first
    }

    host = h;
    port = p;
    if (gps_open(h.toAscii(), QString().setNum(p).toAscii(), &gpsdata) == 0) {
        notifier = new QSocketNotifier(gpsdata.gps_fd, QSocketNotifier::Read);
        notifier->setEnabled(true);
        connect(notifier, SIGNAL(activated(int)), this, SLOT(handleInput(int)));
        emit connectionStatus(true);

        gps_stream(&gpsdata, WATCH_ENABLE|m, NULL);
    } else {
        if (errno) {
            emit connectionError(errno);
        }
    }
}

void Gpsd::disconnectFromServer()
{
    if (!isConnected())
        return;
    
    gps_close(&gpsdata);
    notifier->setEnabled(false);
    delete(notifier);
    emit connectionStatus(false);
}

void Gpsd::setDevice(const QString &d)
{
    device = d;
}

bool Gpsd::isConnected() const
{
    return (gpsdata.gps_fd > 0);
}

const struct gps_data_t* Gpsd::gpsData()
{
    return &gpsdata_ext;
}

bool Gpsd::flag(int f) const
{
    return (gpsdata_ext.set & f);
}

void Gpsd::handleInput(int)
{
    // This updates the gpsdata structure
    // FIXME - In Git repo, this has been renamed to gps_read() already,
    // be sure to link against correct version of library. At the time
    // of writing this (early Sept. 2010) the latest stable GPSD comes
    // with a different, incompatible API, so currently this code can 
    // only be used with a libgps.so built from git HEAD.
    int r = gps_read(&gpsdata);
    if (r == -1) {
        int e = errno;
        disconnectFromServer();
        emit connectionError(e);
        return; // Bail out if no data has been read
    }
    
    // FIXME -- Ideally, it appears that one would use WATCH_DEVICE and
    // the give the device name in the third argument to gps_stream()
    // above to watch a specific device. I only discovered this after
    // this code had been written and did not get around to testing
    // this theory. What I have now appears to work anyway.
    if (!device.isEmpty() && (device != gpsdata.dev.path))
        return; // We're not watching this device
        
    gpsdata_ext = gpsdata; // gpsdata_ext is what we expose outside the class
    emit dataReceived();
    
    if (gpsdata.set & (LATLON_SET|ALTITUDE_SET|SPEED_SET|TRACK_SET|CLIMB_SET)) {
        emit positionUpdated();
    }
    
    if (gpsdata.set & SATELLITE_SET) {
        emit constellationUpdated();
    }
    
    if (gpsdata.set & DEVICELIST_SET) {
        emit deviceListUpdated();
    }
    
    if (gpsdata.set & DEVICEID_SET ||
        gpsdata.set & MODE_SET ||
        gpsdata.set & SATELLITE_SET) {
        // FIXME -- This is probably redundant and needs to go.
        emit dataUpdated();
    }
}

double Gpsd::convertAltitude(double val, AltitudeUnit unit)
{
    switch (unit) {
        case Metre:
            return val;
        case Foot:
            return val * f_Metre2Foot;
    }
    return val;
}

double Gpsd::convertDistance(double val, DistanceUnit unit)
{
    switch (unit) {
        case Kilometre:
            return val * f_Metre2Kilometre;
        case StatuteMile:
            return val * f_Metre2StatuteMile;
        case NauticalMile:
            return val * f_Metre2NauticalMile;
    }
    return val;
}

double Gpsd::convertSpeed(double val, SpeedUnit unit)
{
    switch (unit) {
        case MPS:
            return val;
        case KPH:
            return val * f_MPS2KPH;
        case MPH:
            return val * f_MPS2MPH;
        case Kt:
            return val * f_MPS2Kt;
    }
    return val;
}

// FIXME -- For ease of maintenance this could and should be merged
// with convertLatLon().
QString Gpsd::convertTrack(double val, LatLonUnit unit)
{
    int d, m;       // degrees, minutes
    double s, md;   // seconds, decimal minutes
    switch (unit) {
        case DegreesMinutesSeconds:
            s = fabs(val * 3600.0);
            d = (s - fmod(s, 3600))/3600;
            m = (s - d*3600.0 - fmod(s, 60))/60;
            s = fmod(s, 60);
            
            return QString("%1\xb0%2'%3\"").arg(d, 3, 10, QChar('0'))
            .arg(m, 2, 10, QChar('0'))
            .arg(s, 0, 'f', 3);
            
        case DegreesMinutes:
            md = fabs(val * 60.0);
            d = (md - fmod(md, 60))/60;
            md = fmod(md, 60);
            
            return QString("%1\xb0%2'").arg(d, 3, 10, QChar('0')).arg(md, 6, 'f', 4, QChar('0'));
            
        case Degrees:
            return QString("%1 \xb0").arg(val, 0, 'f', 6);
            
        case Grads:
            return QString("%1 g").arg(val * f_Degrees2Grads, 0, 'f', 6);
            
        case Radians:
            return QString("%1 rad").arg(val * f_Degrees2Radians, 0, 'f', 6);
    }
    return QString("%1").arg(val);
}

QString Gpsd::convertLatLon(bool isLat, double val, LatLonUnit unit)
{
    int d, m;       // degrees, minutes
    double s, md;   // seconds, decimal minutes
    const char *hemispheres[] = { "S", "N", "W", "E" };
    const char *h;
    h = hemispheres[(val < 0? 0 : 1) + (isLat? 0 : 2)];
    switch (unit) {
        case DegreesMinutesSeconds:
            s = fabs(val * 3600.0);
            d = (s - fmod(s, 3600))/3600;
            m = (s - d*3600.0 - fmod(s, 60))/60;
            s = fmod(s, 60);
            
            return QString("%1\xb0%2'%3\" %4").arg(d, isLat?2:3, 10, QChar('0'))
                .arg(m, 2, 10, QChar('0'))
                .arg(s, 0, 'f', 3)
                .arg(h);
            
        case DegreesMinutes:
            md = fabs(val * 60.0);
            d = (md - fmod(md, 60))/60;
            md = fmod(md, 60);
            
            return QString("%1\xb0%2' %3").arg(d, isLat?2:3, 10, QChar('0')).arg(md, 6, 'f', 4, QChar('0')).arg(h);
            
        case Degrees:
            return QString("%1 \xb0").arg(val, 0, 'f', 6);
            
        case Grads:
            return QString("%1 g").arg(val * f_Degrees2Grads, 0, 'f', 6);
            
        case Radians:
            return QString("%1 rad").arg(val * f_Degrees2Radians, 0, 'f', 6);
    }
    return QString("%1").arg(val);
}

const char *Gpsd::suffix(Gpsd::AltitudeUnit unit)
{
    switch (unit) {
        case Metre:
            return "m";
        case Foot:
            return "ft";
    }
    return "";
}

const char *Gpsd::suffix(Gpsd::DistanceUnit unit)
{
    switch (unit) {
        case Kilometre:
            return "km";
        case StatuteMile:
            return "sm";
        case NauticalMile:
            return "nm";
    }
    return "";
}

const char *Gpsd::suffix(Gpsd::SpeedUnit unit)
{
    switch (unit) {
        case MPS:
            return "m/s";
        case KPH:
            return "km/h";
        case MPH:
            return "mi/h";
        case Kt:
            return "kt";
    }
    return "";
}

