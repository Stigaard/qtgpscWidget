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

#ifndef SATTABLE_H
#define SATTABLE_H

#include <QTableWidget>
#include "gpsd.h"

//! Display a table with constellation info.
/*!
* This class displays a table with one row
* per visible satellite. The columns show
* satellite information such as PRN
* (Pseudo-random number, aka satellite ID),
* azimuth, elevation, etc.
*/
class SatTable : public QTableWidget
{

    Q_OBJECT

    public:

        SatTable(QWidget *parent);

        //! Set constellation info.
        /*!
        * \param sats Latest constellation info as received
        * from GPSD server.
        */
        void setSatellites(const SatList &sats);

};

#endif /* SATTABLE_H */
