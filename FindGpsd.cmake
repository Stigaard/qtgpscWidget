# - Find libgps
# Find the Gpsd includes and client library
# This module defines
#  LIBGPS_INCLUDE_DIR, where to find gps.h
#  LIBGPS_LIBRARIES, the libraries needed by a GPSD client.
#  LIBGPS_FOUND, If false, do not try to use GPSD.
# also defined, but not for general use are
#  LIBGPS_LIBRARY, where to find the GPSD library.

#=============================================================================
# Copyright 2010 Diego Berge <gpsd@navlost.eu>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, see <http://www.gnu.org/licenses/>
#=============================================================================

FIND_PATH(LIBGPS_INCLUDE_DIR gps.h)

SET(LIBGPS_NAMES ${LIBGPS_NAMES} gps)
FIND_LIBRARY(LIBGPS_LIBRARY NAMES ${LIBGPS_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set LIBGPS_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(LIBGPS DEFAULT_MSG LIBGPS_LIBRARY LIBGPS_INCLUDE_DIR)

IF(LIBGPS_FOUND)
  SET(LIBGPS_LIBRARIES ${LIBGPS_LIBRARY})
ENDIF(LIBGPS_FOUND)

# Deprecated declarations.
SET (NATIVE_LIBGPS_INCLUDE_PATH ${LIBGPS_INCLUDE_DIR} )
IF(LIBGPS_LIBRARY)
  GET_FILENAME_COMPONENT (NATIVE_LIBGPS_LIB_PATH ${LIBGPS_LIBRARY} PATH)
ENDIF(LIBGPS_LIBRARY)

MARK_AS_ADVANCED(LIBGPS_LIBRARY LIBGPS_INCLUDE_DIR )
