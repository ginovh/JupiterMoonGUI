#!/bin/bash
rm -rf aaplus
mkdir aaplus
cd aaplus
wget http://www.naughter.com/download/aaplus.zip
unzip aaplus.zip
patch -p1 < ../aaplus.patch
cmake .
make -j4
