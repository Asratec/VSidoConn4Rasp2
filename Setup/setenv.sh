#!/bin/bash

opkg update 
opkg install v4l-utils v4l-utils-dev libv4l-dev libv4l 
opkg install git
opkg install cmake
opkg install opencv-dev
sync
sync




