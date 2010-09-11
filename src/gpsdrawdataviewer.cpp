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

#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QScrollBar>
#include "gpsdrawdataviewer.h"

GpsdRawDataViewer::GpsdRawDataViewer(QWidget *parent)
    : QWidget(parent)
{
    host = QString();
    port = 0;
    format = Gpsd::None;
    
    ui.setupUi(this);
    
    ui.rawDataFormat->addItem("None", QVariant((int)Gpsd::None));
    ui.rawDataFormat->addItem("JSON", QVariant((int)Gpsd::Json));
    ui.rawDataFormat->addItem("NMEA", QVariant((int)Gpsd::Nmea));
    ui.rawDataFormat->addItem("Raw", QVariant((int)Gpsd::Hex));
    
    ui.rawDataClear->setDefaultAction(ui.actionClear);
    ui.rawDataCopy->setDefaultAction(ui.actionCopy);
    ui.rawDataSave->setDefaultAction(ui.actionSave);
    
    connect(ui.wrapText, SIGNAL(toggled(bool)), this, SLOT(wrapText(bool)));
    connect(ui.actionClear, SIGNAL(triggered(bool)), ui.rawDataViewer, SLOT(clear()));
    connect(ui.actionCopy, SIGNAL(triggered(bool)), this, SLOT(copyToClipboard()));
    connect(ui.actionSave, SIGNAL(triggered(bool)), this, SLOT(saveToFile()));
    connect(ui.rawDataFormat, SIGNAL(activated(int)), this, SLOT(switchFormat(int)));
    connect(&gpsd, SIGNAL(dataUpdated()), this, SLOT(updateData()));
}

void GpsdRawDataViewer::setHost(const QString &h)
{
    host = h;
}

void GpsdRawDataViewer::setPort(quint16 p)
{
    port = p;
}

void GpsdRawDataViewer::setFormat(Gpsd::StreamMode f)
{
    format = f;
    if (format == Gpsd::None) {
        gpsd.disconnectFromServer();
    } else if (!host.isEmpty() && port != 0) {
        gpsd.connectToServer(host, port, f);
    }
}

void GpsdRawDataViewer::connectToServer(bool c)
{
    if (c && !host.isEmpty() && port != 0) {
        ui.rawDataFormat->setEnabled(true);
        if (format != Gpsd::None) {
            gpsd.connectToServer(host, port, format);
        }
    } else {
        ui.rawDataFormat->setEnabled(false);
        gpsd.disconnectFromServer();
    }
}

void GpsdRawDataViewer::disconnectFromServer()
{
    gpsd.disconnectFromServer();
}

void GpsdRawDataViewer::updateData()
{
    QRegExp regexNmea("^\\$.*\\*..$");      // Regex to detect an NMEA sentence
    QRegExp regexHex("^([0-9a-f]{2})+$");   // Regex to detect an hex raw data string
    QStringList lines;  // Data packets can consist of one or more lines, so we split
                        // them here. Presumably incomplete lines might also arrive. If
                        // that's the case we just ignore them, otherwise we'll end up
                        // complicating the code more than it needs to.
    QString line;
    
    switch (format) {
        case Gpsd::Json:
            ui.rawDataViewer->appendPlainText(gpsd.gpsData()->buffer);
            break;
        case Gpsd::Nmea:
            lines = QString(gpsd.gpsData()->buffer).split("\n");
            foreach (line, lines) {
                line = line.trimmed();
                if (regexNmea.exactMatch(line)) {
                    ui.rawDataViewer->appendPlainText(line);
                } else {
                }
            }
            break;
        case Gpsd::Hex:
            lines = QString(gpsd.gpsData()->buffer).split("\n");
            foreach (line, lines) {
                line = line.trimmed();
                if (regexHex.exactMatch(line)) {
                    QString l;
                    int n = ui.rawDataViewer->toPlainText().length() - ui.rawDataViewer->toPlainText().lastIndexOf("\n") - 1;
                    n = (n - n/24)/3;
                    for (int i=0; i<line.length(); i+=2) {
                        n++;
                        l.append(line.mid(i, 2));
                        l.append((n%24)?((n%8)?" ":"  "):" \n");
                    }
                    ui.rawDataViewer->textCursor().movePosition(QTextCursor::End);
                    ui.rawDataViewer->insertPlainText(l);
                }
            }
            break;
        default:
            // Do nothing
            break;
    }
    ui.rawDataViewer->verticalScrollBar()->setValue(
        ui.rawDataViewer->verticalScrollBar()->maximum()
    );
}

void GpsdRawDataViewer::switchFormat(int idx)
{
    if (idx == -1) {
        ui.rawDataFormat->setCurrentIndex(0);
        return;
    }
    
    setFormat((Gpsd::StreamMode)ui.rawDataFormat->itemData(idx).toInt());
}

void GpsdRawDataViewer::wrapText(bool wrap)
{
    if (wrap) {
        ui.rawDataViewer->setLineWrapMode(QPlainTextEdit::WidgetWidth);
    } else {
        ui.rawDataViewer->setLineWrapMode(QPlainTextEdit::NoWrap);
    }
}

void GpsdRawDataViewer::copyToClipboard()
{
    ui.rawDataViewer->selectAll();
    ui.rawDataViewer->copy();
}

void GpsdRawDataViewer::saveToFile(const QString &fn)
{
    QString fileName;
    if (fn.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, "Save Raw Data As...");
        if (fileName.isEmpty())
            return;
    } else {
        fileName = fn;
    }
    
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly|QIODevice::Text)) {
        QMessageBox::critical(this, "File Saving Error", QString("Unable to open file.\n%1").arg(file.errorString()));
        return;
    }
    
    if (file.write(ui.rawDataViewer->toPlainText().toAscii()) == -1) {
        QMessageBox::critical(this, "File Saving Error", QString("Unable to save file.\n%1").arg(file.errorString()));
    }
    file.close();
}
