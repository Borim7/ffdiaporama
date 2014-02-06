#!/bin/bash
: << _COMMENT_
    ===========================================================================
    This file is part of ffDiaporama
    ffDiaporama is a tools to make diaporama as video
    Copyright (C) 2011-2014 Dominique Levray <domledom@laposte.net>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    ===========================================================================

    This script will install openclipart 0.18 on your computer

    Syntaxe: sudo setup.bash [ROOT DESTINATION]
        ROOT DESTINATION is used when creating package (example: $RPM_BUILD_ROOT)
    ===========================================================================

    SCRIPT DATE VERSION: Version 0.18 20140205

    ===========================================================================
_COMMENT_

if [[ $EUID -ne 0 ]]; then
  echo "You must be a root user" 2>&1
  exit 1
fi

#==============================================================================

if [ -d /usr/share/clipart/openclipart-0.18 ]; then
    # openSUSE package "openclipart-svg" install files in /usr/share/clipart/openclipart-0.18
    mkdir $1/usr/share/ffDiaporama/clipart/openclipart-0.18-svgonly/
    ln -s /usr/share/clipart/openclipart-0.18/ $1/usr/share/ffDiaporama/clipart/openclipart-0.18-svgonly/clipart
elif [ -d /usr/share/clipart/openclipart ]; then
    # Fedora package "openclipart" install files in /usr/share/openclipart
    mkdir $1/usr/share/ffDiaporama/clipart/openclipart-0.18-svgonly/
    ln -s /usr/share/clipart/openclipart $1/usr/share/ffDiaporama/clipart/openclipart-0.18-svgonly/clipart
else
    curl -o openclipart-0.18-svgonly.zip http://ftp.osuosl.org/pub/openclipart/downloads/0.18/openclipart-0.18-svgonly.zip
    if [[ ! -f openclipart-0.18-svgonly.zip ]]; then
        echo Impossible to get ffDiaporama-openclipart from ffDiaporama Web site
        cd ~
        exit 1
    fi
    unzip openclipart-0.18-svgonly.zip -d /usr/share/ffDiaporama/clipart/
fi

#==============================================================================

cp openclipart-0.18-svgonly-licence.txt $1/usr/share/ffDiaporama/clipart/
cp openclipart-0.18-svgonly-readme.txt $1/usr/share/ffDiaporama/clipart/
cp OCTBUILDVERSION.txt $1/usr/share/ffDiaporama/clipart/
