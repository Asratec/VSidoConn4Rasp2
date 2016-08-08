VSID_RC := vsido.connect.rc
VSID_RC += vsido.webcam.rc
VSID_RC += vsido.streamingraw.rc
VSID_RC += vsido.streamingmarker.rc
VSID_RC += vsido.streamingsub.rc
#VSID_RC += vsido.ifttt.rc

VSID_NGINX := nginx.vsido.web.game
VSID_NGINX += nginx.vsido.web.root
VSID_NGINX += nginx.vsido.web.server
VSID_NGINX += nginx.vsido.web.debug


.PHONY:service $(VSID_RC) $(VSID_NGINX) inittab.cp

service:copy.rc copy.nginx inittab.cp $(VSID_RC) $(VSID_NGINX)
	sync
copy.rc:
	cp -f ./vsido.*.rc /etc/init.d/
	chmod +x /etc/init.d/vsido.*
	
$(VSID_RC):
	echo "set to auto boot $@"
	update-rc.d -f $@ defaults

copy.nginx:
	cp -f ./nginx.* /etc/nginx/sites-available/
	
inittab.cp:
	cp -f ./inittab /etc/inittab
		
$(VSID_NGINX):
	echo "set to auto boot $@"
	cd /etc/nginx/sites-enabled/ && ln -sf /etc/nginx/sites-available/$@ $@ 
	