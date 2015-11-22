#!/bin/bash

apt-get update 
version=`cat /etc/debian_version | cut -b1-1`
if [ $version -lt 8 ] ;then
	apt-get install -y gcc-4.8 g++-4.8 --fix-missing
	cd /usr/bin && ln -sf gcc-4.8 gcc
	cd /usr/bin && ln -sf g++-4.8 g++
fi
apt-get install -y nginx git cmake libv4l-dev libopencv-dev rfkill bluez bluez-hcidump --fix-missing

sync
sync

