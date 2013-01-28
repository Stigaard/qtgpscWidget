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

#ifndef SATVIEW_H
#define SATVIEW_H

#include <QWidget>
#include <QTransform>
#include <QList>
#include "satellite.h"



//! Display a polar plot of visible satellites
/*!
* This class implements a widget showing a simple
* polar plot of satellites. Visible satellites are
* shown with a fill if they are flagged as used in
* the position solution, and hollow otherwise. The
* PRN numbers are shown next to each satellite in
* any event.
*/
class SatView : public QWidget
{

    Q_OBJECT

    public:
        SatView(QWidget *parent = 0);

        //! Set constellation info.
        /*!
        * \param sats Latest constellation info as received
        * from GPSD server.
        */
        void setSatellites(const SatList &sats);

    protected:
        //! Reimplemented from QWidget
        void paintEvent(QPaintEvent *event);
        //! Show a custom context menu
        void contextMenuEvent(QContextMenuEvent *event);
        //! Show a tooltip with polar coordinates
        void mouseMoveEvent(QMouseEvent *event);
        void resizeEvent(QResizeEvent *event);
        
    private slots:
        //! Called by the context menu, copies the
        //! plot to the keyboard as an image.
        void copyToClipboard();
        
    private:
        SatList satellites;     // List of satellites
        QAction *actionCopy;    // Copy to clipboard action
        
        QTransform transform;   // Transformation matrix
        
        // Utility functions, polar <-> rectangular conversions
        static QPointF polar2rect(double azm, double el);
        static QPointF rect2polar(double x, double y);
        static QPointF rect2polar(QPointF r);
};

#endif /* SATVIEW_H */
