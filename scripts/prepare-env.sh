#!/bin/bash

sudo apt-get install git libcppunit-dev make g++ build-essential unzip cmake \
                     pkg-config libboost-thread-dev libtbb-dev \
                     libavcodec-dev libavformat-dev libswscale-dev \
                     gcovr nodejs npm

npm install ws

cd webdash/railsapp/webdash
bundle install

if [ ! -d "/usr/local/include/opencv" ]; then
  echo "$0: error: need opencv. Download and install."
  echo "e.g.:"
  echo "  wget http://downloads.sourceforge.net/project/opencvlibrary/opencv-unix/2.4.10/opencv-2.4.10.zip?r=http%3A%2F%2Fsourceforge.net%2Fprojects%2Fopencvlibrary%2F&ts=1417908259&use_mirror=jaist"
  echo "  unzip opencv-2.4.*"
  echo "  mkdir opencv-build"
  echo "  cd opencv-build"
  echo "  mkdir release"
  echo "  cd release"
  echo "  cmake -D CMAKE_BUILD_TYPE=RELEASE -D CMAKE_INSTALL_PREFIX=/usr/local ../../opencv-2.4.10"
  echo "  make"
  echo "  sudo make install"
  echo "  sudo ldconfig"
  exit -1
fi

echo "All packages and OpenCV seem to be installed."

