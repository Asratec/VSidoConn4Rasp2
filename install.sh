#!/bin/bash
sudo apt-get update 
version=`cat /etc/debian_version | cut -b1-1`
if [ $version -lt 8 ] ;then
	apt-get install -y gcc-4.8 g++-4.8 --fix-missing
	cd /usr/bin && ln -sf gcc-4.8 gcc
	cd /usr/bin && ln -sf g++-4.8 g++
fi
sudo apt-get install -y --fix-missing nginx git cmake rfkill bluez bluez-hcidump libv4l-dev libopencv-dev nodejs
wget --no-check-certificate https://asratec.github.io/VSidoConn4Rasp2/Binary/v0.96/VSidoConn4Rasp2.tar.gz -O VSidoConn4Rasp2.tar.gz 
sudo mkdir -p /opt/vsido/
sudo tar -xzvf VSidoConn4Rasp2.tar.gz -C /opt/vsido/
sudo make -C /opt/vsido/usr/share/Config
sync
sync
