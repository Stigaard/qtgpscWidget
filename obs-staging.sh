#!/bin/bash
#   Copyright (C) 2010 by Diego Berge <devel@navlost.eu>
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

# This script creates the pristine source tarball and spec, dsc, and other
# packaging files for upload to the openSUSE Build Service for building and
# package generation. Run it from the top-level project directory. Output
# is placed into "staging" subdirectory, overwriting what was there.

if [ ! -d staging  ]
then
    mkdir staging
fi

# Check exit code passed as a parameter and display a message if it's not 0 (success).
# $1: The exit code value.
# $2: Message to display if it's non-zero.
check_errs()
{
  if [ "${1}" -ne "0" ]; then
    echo "ERROR # ${1} : ${2}"
    exit ${1}
  fi
}

# Clear output directory.
[ -d staging ] && rm staging/*

# Create pristine source tarball.
VERSION=`cat VERSION`
BASE_NAME=qtgpsc-$VERSION
OPENSUSE113_SPEC=$BASE_NAME-openSUSE_11.3.spec
echo Creating $BASE_NAME.tar.gz.
tar cvfz staging/$BASE_NAME.tar.gz \
    --exclude './.*' --exclude 'staging/**' --exclude 'build/**' \
    --transform s\\^\\$BASE_NAME/\\g * >/dev/null
check_errs $? "Can't create archive $BASE_NAME"

# Create RPM specs for openSUSE.
echo "Creating RPM spec."
sed <obs/qtgpsc_basic.spec -e s/@VERSION@/$VERSION/g >staging/$BASE_NAME.spec
check_errs $? "Can't generate spec file."
cp obs/qtgpsc.changes staging/$BASE_NAME.changes

# # Create Ubuntu files. Current version of Debian (5.0) in OBS does not
# # have all required dependencies
# echo "Creating Debian changelog."
# sed <obs/debian.changelog -e s/@DEB_VERSION@/$VERSION/g -e s/@DISTRO@/debian/g >staging/debian.changelog
# echo "Creating Debian dsc."
# sed <obs/qtgpsc.dsc -e s/@DEB_VERSION@/$VERSION/g >staging/qtgpsc.dsc
# echo "Creating Ubuntu dsc."
# sed <obs/qtgpsc.dsc -e s/@DEB_VERSION@/$VERSION/g >staging/qtgpsc-xUbuntu_10.04.dsc
# echo "Copying other Debian / Ubuntu files."
# cp obs/debian.control* obs/debian.rules staging

# Sign source tarball
echo "Signing source tarball"
gpg2 -bau "CC71A1E844826282F4A994F29A5284B525AB6E3D" --sign -o staging/$BASE_NAME.tar.gz.sig staging/$BASE_NAME.tar.gz
