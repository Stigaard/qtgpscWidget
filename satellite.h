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

#ifndef SATELLITE_H
#define SATELLITE_H

#include <QList>

//! Support class for Gpsd. Holds position and status data for one satellite
class Satellite {
    
    public:
        
        Satellite();
        
        Satellite(int prn, double azm, double ele,
                  double snr, bool used, bool healthy);
                  
    public:
        int prn;
        double azm, ele, snr;
        bool used, healthy;
};

//! Convenience type to hold a list of satellites
typedef QList<Satellite> SatList;

#endif /* SATELLITE_H */
