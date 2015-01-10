#!/bin/bash

# Find cocos root
SCRIPT=$(readlink -f "$0")
COCOS_DIR=$(dirname $(dirname ${SCRIPT}))

if [ ! $(command -v apt-get) ]; then
  echo "Not a .deb package system. Please install dependencies manually"
  exit 0
fi

#install g++-4.9
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt-get update

#try to remove glfw2
sudo apt-get remove libglfw2

DEPENDS='libx11-dev'
DEPENDS+=' libxmu-dev'
DEPENDS+=' libglu1-mesa-dev'
DEPENDS+=' libgl2ps-dev'
DEPENDS+=' libxi-dev'
DEPENDS+=' g++-4.9'
DEPENDS+=' libzip-dev'
DEPENDS+=' libpng12-dev'
DEPENDS+=' libcurl4-gnutls-dev'
DEPENDS+=' libfontconfig1-dev'
DEPENDS+=' libsqlite3-dev'
DEPENDS+=' libglew-dev'
DEPENDS+=' libssl-dev'

MISSING=
echo "Checking for missing packages ..."
for i in $DEPENDS; do
    if ! dpkg-query -W --showformat='${Status}\n' $i | grep "install ok installed" > /dev/null; then
        MISSING+="$i "
    fi
done


if [ -f /usr/bin/g++ ];then
sudo rm /usr/bin/g++
echo "remove old g++"
fi
sudo ln -s /usr/bin/g++-4.9 /usr/bin/g++

if [ -n "$MISSING" ]; then
    TXTCOLOR_DEFAULT="\033[0;m"
    TXTCOLOR_GREEN="\033[0;32m"
    echo -e $TXTCOLOR_GREEN"Missing packages: $MISSING.\nYou may be asked for your password for package installation."$TXTCOLOR_DEFAULT
    sudo apt-get --force-yes --yes install $MISSING
fi

# install glfw
${COCOS_DIR}/tools/travis-scripts/install_glfw.sh

