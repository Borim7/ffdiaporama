#!/bin/bash

if [[ $EUID -ne 0 ]]; then
  echo "You must be a root user" 2>&1
  exit 1
fi

#==============================================================================

if ! wget -t 0 http://ftp.osuosl.org/pub/openclipart/downloads/0.18/openclipart-0.18-svgonly.zip -O openclipart-0.18-svgonly.zip; then
    echo Impossible to get ffDiaporama-openclipart from ffDiaporama Web site
    cd ~
    exit 1
else
    unzip openclipart-0.18-svgonly.zip -d /usr/share/ffDiaporama/clipart/
fi

#==============================================================================

cp openclipart-0.18-svgonly-licence.txt /usr/share/ffDiaporama/clipart/
cp openclipart-0.18-svgonly-readme.txt /usr/share/ffDiaporama/clipart/
