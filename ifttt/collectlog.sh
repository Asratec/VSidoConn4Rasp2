#!/usr/bin/env bash
umount /mnt/vsido/log2
mkdir -p /mnt/vsido/log2
mount -t tmpfs -o size=2M tmpfs /mnt/vsido/log2

while true
do
	sleep 60
	#sleep 10
	name=`cat /mnt/vsido/log/*.log | md5sum | cut -c 1-32`
	cat /mnt/vsido/log/*.log >/mnt/vsido/log2/${name}.txt
	#find ./
	/opt/vsido/usr/bin/dropbox_uploader.sh -s -f /opt/vsido/usr/bin/.dropbox_uploader upload  /mnt/vsido/log2/${name}.txt /robot-log/
done

