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

#include <QApplication>
#include <QTimer>
#include <QUrl>
#include "qtgpsc.h"

void processCommandLine(QtGpsc &qtgpsc)
{
    if (qApp->argc() == 2) {
        QUrl url(qApp->argv()[1]);
        if (url.isValid() && (url.scheme() == "gpsd" || url.scheme() == QString())) {
            bool signalsBlocked = qtgpsc.blockSignals(true);
            if (url.host() != QString())
                qtgpsc.setHost(url.host());
            if (url.port() > 0)
                qtgpsc.setPort(url.port());
            if (url.path() != QString() && url.path() != "/" && qtgpsc.isConnected())
                qtgpsc.setDevice(url.path());
            qtgpsc.blockSignals(signalsBlocked);
            qtgpsc.connectToServer();
        }
    }
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QtGpsc qtgpsc;
    processCommandLine(qtgpsc);
    qtgpsc.show();
    return app.exec();
}
