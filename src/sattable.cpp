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

#include "sattable.h"

SatTable::SatTable(QWidget *parent)
    : QTableWidget(parent)
{
    setRowCount(0);
}

void SatTable::setSatellites(const SatList  &sats)
{
    Satellite sat;
    setRowCount(0);

    setUpdatesEnabled(false);
    // Populate table
    foreach (sat, sats) {
        int n = rowCount();
        setRowCount(n+1);
        QTableWidgetItem *i;
        i = new QTableWidgetItem(QString().sprintf("%03d", sat.prn));
        setItem(n, 0, i);
        i = new QTableWidgetItem(QString().sprintf("%03.0f", sat.azm));
        setItem(n, 1, i);
        i = new QTableWidgetItem(QString().sprintf("%03.0f", sat.ele));
        setItem(n, 2, i);
        i = new QTableWidgetItem(QString::number(sat.snr));
        setItem(n, 3, i);
        i = new QTableWidgetItem(QString(sat.used?"Yes":"No"));
        setItem(n, 4, i);
    }
    
    sortItems(0);   // Sort by PRN
    resizeColumnsToContents();
    setUpdatesEnabled(true);
}
