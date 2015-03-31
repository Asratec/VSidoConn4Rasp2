#!/bin/bash

apt-get update 
apt-get install -y gcc-4.8 g++-4.8 --fix-missing
apt-get install -y nginx git cmake --fix-missing
apt-get install -y libv4l-dev libopencv-dev --fix-missing
cd /usr/bin && ln -sf gcc-4.8 gcc
cd /usr/bin && ln -sf g++-4.8 g++
sync
sync

wget --no-check-certificate https://asratec.github.io/VSidoConn4Rasp2/Binary/v0.81/VSidoConn4Rasp2.tar.gz -O VSidoConn4Rasp2.tar.gz 
sudo mkdir -p /opt/vsido/
sudo tar -xzvf VSidoConn4Rasp2.tar.gz -C /opt/vsido/
sudo make -C /opt/vsido/usr/share/Config

wget --no-check-certificate https://asratec.github.io/VSidoConn4Rasp2/Binary/node-v0.10.28-linux-arm-pi.tar.gz -O node-v0.10.28-linux-arm-pi.tar.gz 
sudo mkdir -p /opt/vsido/
sudo tar -xzvf node-v0.10.28-linux-arm-pi.tar.gz -C /opt/vsido/
sudo cp -rf /opt/vsido/node-v0.10.28-linux-arm-pi/* /usr/


sync
sync



