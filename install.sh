#!/bin/bash
wget --no-check-certificate https://asratec.github.io/VSidoConn4Rasp2/Binary/v0.81/VSidoConn4Rasp2.tar.gz -O VSidoConn4Rasp2.tar.gz 
sudo mkdir -p /opt/vsido/
sudo tar -xzvf VSidoConn4Rasp2.tar.gz -C /opt/vsido/
sudo make -C /opt/vsido/usr/share/Config
sync
sync



