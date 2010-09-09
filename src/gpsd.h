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

#ifndef GPSD_H
#define GPSD_H

#include <QObject>
#include <QSocketNotifier>
#include "gps.h"

#if GPSD_API_MAJOR_VERSION < 5
#error \
"Unsupported GPSD API version.\n"\
"This program requires GPSD_API_MAJOR_VERSION >= 5, please\n"\
"upgrade your GPSD libraries or get the latest GPSD source\n"\
"from http://gpsd.berlios.de/"
#endif

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

//! Wrapper for the libgps library.
class Gpsd : public QObject
{

    Q_OBJECT
    
    Q_ENUMS(StreamMode AltitudeUnit DistanceUnit LatLonUnit SpeedUnit)
    
    Q_PROPERTY(QString host READ getHost)
    Q_PROPERTY(quint16 port READ getPort)
    Q_PROPERTY(QString device READ getDevice WRITE setDevice)

    public:
        
        //! Used to specify the streaming mode.
        /*!
        * i.e., whether the GPSD daemon should send NMEA or raw data.
        * The values for the enum members are taken from the corresponding
        * #defines in gps.h, with the exception of \a None.
        */
        enum StreamMode   { None = 0, Json = WATCH_NEWSTYLE, Nmea = WATCH_NMEA, Hex = WATCH_RARE, Raw = WATCH_RAW };
        
        //! \enum AltitudeUnit
        //! \enum DistanceUnit
        //! \enum LatLonUnit
        //! \enum SpeedUnit
        //! Various units definitions.
        /*!
        * Internally everything is kept on the units used by GPSD, but
        * various static functions are provided for conversion between
        * units, and they use the following enums.
        */
        enum AltitudeUnit { Metre, Foot };
        enum DistanceUnit { Kilometre, StatuteMile, NauticalMile };
        enum LatLonUnit   { DegreesMinutesSeconds, DegreesMinutes, Degrees, Grads, Radians };
        enum SpeedUnit    { MPS, KPH, MPH, Kt };
        
        Gpsd(QObject *parent = 0);
        ~Gpsd();
        
        const QString getHost() const { return host; };
        quint16 getPort() const { return port; };
        const QString getDevice() const { return device; };
        
        //! Attempts to establish a connection with the GPSD server
        /*!
        * This function attempts to connect to a GPSD server and if
        * successful, starts streaming data. One or more signals will
        * be emitted when data is received, depending on the type of
        * information returned.
        * \param host The hostname to connect to.
        * \param port The port number to connect to.
        * \param mode The streaming mode to use.
        */
        void connectToServer(const QString &host = "localhost", quint16 port = 2947, StreamMode mode = Json);
        void disconnectFromServer(); //!< Disconnects from server
        bool isConnected() const;    //!< \return \c true is connected to a GPSD server, \c false otherwise.
        const struct gps_data_t* gpsData(); //!< \return A const pointer to the \c gps_data_t struct.
        /*! Utility function to check if a given data member has been
        * populated by the previous fix in \c gps_data_t.
        * \param f The *_SET flag to check, as defined in gps.h
        * \return \c true if the \p f flag is set, \c false otherwise.
        * Example:
        * \code
        * if flag(LATLON_SET) {
        *   // a position update has been received,
        *   // process position update.
        * }
        * \endcode
        */
        bool flag(int f) const;
        
        static double convertAltitude(double val, AltitudeUnit unit); //!< Utility function to convert altitudes
        static double convertDistance(double val, DistanceUnit unit); //!< Utility function to convert distances
        static double convertSpeed(double val, SpeedUnit unit); //!< Utility function to convert speeds
        //! Utility function to represent angles in different formats.
        static QString convertTrack(double val, LatLonUnit unit);
        //! Utility function to represent geographic positions in different formats.
        /*!
        * \param isLat Set to \c true if \p val is a latitude value.
        * \param val The value to convert, in fractional sexagesimal degrees.
        * \param unit The output format
        * \return A QString representation of \p val in the requested format.
        * \see LatLonUnit
        */
        static QString convertLatLon(bool isLat, double val, LatLonUnit unit);
        
        static const char *suffix(AltitudeUnit);//!< Utility function that returns an altitude unit suffix
        static const char *suffix(DistanceUnit);//!< Utility function that returns a distance unit suffix
        static const char *suffix(SpeedUnit);   //!< Utility function that returns a speed unit suffix
        
    public slots:
        //! Set the device to watch
        /*!
        * If this function is not used, or an empty QString \p device
        * is supplied, data updates from every device connected to
        * the target GPSD server will be received, otherwise only
        * those updates coming from the specified device will be
        * received.
        * \param device The name of the device to watch, e.g., \e /dev/ttyS0
        */
        void setDevice(const QString &device);
        
    signals:
        void dataReceived();            //!< Emitted when any kind of data has been received.
        void positionUpdated();         //!< Emitted when position, velocity, or time data has been received.
        void constellationUpdated();    //!< Emitted when satelitte constellation data has been received.
        void deviceListUpdated();       //!< Emitted whenever a new device is plugged in, or an old one removed.
        /*! Emitted when one of the following flags has been set:
        * \arg DEVICEID_SET
        * \arg MODE_SET
        * \arg USED_SET
        * \bug This signal is probably redundant and should be removed,
        * \bug since its functionality is already covered by dataReceived()
        */
        void dataUpdated();
        /*! Emitted whenever the connection status changes.
        * \param isConnected is \c true if connected or \c false otherwise.
        */
        void connectionStatus(bool isConnected);
        /*! Emitted whenever GPSD encounters an error condition, such as
        * an unsuccessful or broken connection.
        * \param err The error number, use gps_errstr() to obtain a textual
        * \param err error message.
        */
        void connectionError(int err);
        
    private slots:
        //! Process incoming data.
        /*!
        * This function is called whenever new data has arrived on the socket.
        * Its function is to read the new data into the \c gps_data_t data
        * structure via a call to gps_read(), and emit any and all relevant
        * signals.
        */
        void handleInput(int);
        
    private:
        QString host;
        quint16 port;
        QString device;
        // gpsdata is used internally, gpsdata_ext is what is presented to the
        // user via gpsData().
        struct gps_data_t gpsdata, gpsdata_ext;
        QSocketNotifier *notifier;
        
        // Various conversion factors.
        static const double f_Metre2Foot = 3.2808399;
        static const double f_Metre2Kilometre = 0.001;
        static const double f_Metre2StatuteMile = 0.000621373;
        static const double f_Metre2NauticalMile = 0.000539957;
        static const double f_MPS2KPH = 3.6;
        static const double f_MPS2MPH = 2.23694;
        static const double f_MPS2Kt = 1.94385;
        static const double f_Degrees2Grads = (1/0.9);
        static const double f_Degrees2Radians = 0.017453292519943;
        
};

#endif /* GPSD_H */
