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

#ifndef GPSD_RAW_DATA_VIEWER_H
#define GPSD_RAW_DATA_VIEWER_H

#include <QWidget>
#include "gpsd.h"
#include "ui_GpsdRawDataViewer.h"

//! Raw data viewer
/*!
* This class implements a custom widget used to
* display the GPSD raw data stream in JSON, NMEA
* or raw (hex) formats in a text pane. It also
* has facilities to clear the data from the text
* pane, copy it to the clipboard or save it to a
* file.
*/
class GpsdRawDataViewer : public QWidget
{

    Q_OBJECT
    
    Q_PROPERTY(QString host READ getHost WRITE setHost)
    Q_PROPERTY(quint16 port READ getPort WRITE setPort)
    Q_PROPERTY(Gpsd::StreamMode format READ getFormat WRITE setFormat)

    public:

        GpsdRawDataViewer(QWidget *parent = 0);
        
        const QString getHost() const { return host; };
        quint16 getPort() const { return port; };
        Gpsd::StreamMode getFormat() const { return format; };
        
    public slots:
        void setHost(const QString &h);
        void setPort(quint16 port);
        void setPort(int port) { setPort((quint16)port); };
        void setFormat(Gpsd::StreamMode f);
        void connectToServer(bool c = true);
        void disconnectFromServer();
        void wrapText(bool wrap);
        void copyToClipboard();
        void saveToFile(const QString &fileName = QString());
        
    protected slots:
        void updateData();
        void switchFormat(int idx);
        
    private:
        Ui::GpsdRawDataViewer ui;
        Gpsd::StreamMode format;
        Gpsd gpsd;
        QString host;
        quint16 port;
        

};

#endif /* GPSD_RAW_DATA_VIEWER_H */
