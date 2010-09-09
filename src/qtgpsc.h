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

#ifndef QTGPSC_H
#define QTGPSC_H

#include <QMainWindow>
#include "gpsd.h"
#include "ui_MainWindow.h"

// FIXME -- This define should be replaced by something else which
// gets autoupdated on version number changes.
#define QTGPSC_VERSION ("0.3.0")

//! Application main window class
class QtGpsc : public QMainWindow
{
    Q_OBJECT
    
    public:
        
        QtGpsc(QWidget *parent = 0);
        
        //! \see Gpsd::isConnected()
        bool isConnected() const { return gpsd.isConnected(); };
        
    signals:
        void settingsChanged(); //!< Emitted whenever a persistent setting is changed.
        
    public slots:
        void readSettings();    //!< Read persistent settings from storage.
        void writeSettings();   //!< Write persistent settings to storage.
        void setHost(const QString &host);  //!< Set the target host name. Must be in disconnected state to use.
        void setPort(quint16 port);         //!< Set the target port number. Must be in disconnected state to use.
        void setDevice(const QString &device);  //!< Set the target device to watch.
        //! Attempts to connect to GPSD server.
        //! \see handleConnection(bool conn)
        void connectToServer() { ui.connectButton->setChecked(true); };
        //! Disconnects from GPSD server.
        //! \see handleConnection(bool conn)
        void disconnectFromServer() { ui.connectButton->setChecked(false); };
        
    protected slots:
        
        //! Connects and disconnects from GPSD server
        /*!
        * \param conn If \c true, connect to server, if \c false disconnect.
        * \see Gpsd::connectToServer()
        * \see Gpsd::disconnectFromServer()
        */
        void handleConnection(bool conn);
        //! Take error handling action.
        /*!
        * Writes a message on the status bar with an error description
        */
        void handleError(int err);
        void updatePosition();      //!< Update the Position tab
        void updateConstellation(); //!< Update the Satellites tab
        void updateDeviceList();    //!< Update the device list combo box
        void updateData();          //!< Update the status bar and window title
        
        void copyAsKml();           //!< Copy data from the Position tab as KML
        void copyAsCsv();           //!< Copy data from the Position tab as comma separated values
        
        void unitsChanged(bool checked);    //!< Handle a change in output units selection
        void aboutBox();                    //!< Show the "About" message
        
    private:
        
        Ui::MainWindow ui;
        Gpsd gpsd;
        Gpsd::AltitudeUnit altitudeUnits;
        Gpsd::DistanceUnit distanceUnits;
        Gpsd::SpeedUnit speedUnits;
        Gpsd::LatLonUnit latLonUnits;
};

#endif /* QTGPSC_H */
