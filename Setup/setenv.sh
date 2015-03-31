#!/bin/bash

apt-get update 
apt-get install -y gcc-4.8 g++-4.8 --fix-missing
apt-get install -y nginx git cmake --fix-missing
apt-get install -y libv4l-dev libopencv-dev --fix-missing
cd /usr/bin && ln -sf gcc-4.8 gcc
cd /usr/bin && ln -sf g++-4.8 g++
sync
sync




