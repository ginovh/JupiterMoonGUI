#!/bin/bash
rm -rf aaplus
mkdir aaplus
cd aaplus
wget http://www.naughter.com/download/aaplus.zip
unzip aaplus.zip
chmod +w AAGalileanMoons.*
dos2unix AAGalileanMoons.* # to avoid patch "different line endings error", don't know cleaner method
patch -p1 < ../aaplus.patch
mkdir build
cd build/
cmake ..
#cmake --build .
make -j 4
