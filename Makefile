#Copyright (c) 2015, Asratec Corp.
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or without
#modification, are permitted provided that the following conditions are met:
#
#* Redistributions of source code must retain the above copyright notice, this
#  list of conditions and the following disclaimer.
#
#* Redistributions in binary form must reproduce the above copyright notice,
#  this list of conditions and the following disclaimer in the documentation
#  and/or other materials provided with the distribution.
#
#* Neither the name of VSidoConn4Edison nor the names of its
#  contributors may be used to endorse or promote products derived from
#  this software without specific prior written permission.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
#FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
#DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
#SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
#OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
#OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

OBJROOT  := $(shell pwd)/objects
SRCROOT  := $(shell pwd)/
PACKROOT := $(shell pwd)/package
INSTALLROOT := /home/sysroot/usr


.PHONY: all build install package clean
all:package

build: pre-build
	make -C VSidoConnServer build
	cd $(OBJROOT) && make
install:
	make -C VSidoConnServer install
	cd $(OBJROOT) && make install

package:pre-package
	make -C VSidoConnServer package
	tar xzvf VSidoConnServer/VSidoConnServer.tar.gz -C $(PACKROOT)/
	cd $(OBJROOT) && make install
	tar czvf VSidoConn4Rasp2.tar.gz -C $(PACKROOT) ./

clean:
	make -C VSidoConnServer clean
	rm -rf $(OBJROOT) $(PACKROOT)
	rm -f VSidoConn4Rasp2.tar.gz


pre-build:
	mkdir -p $(OBJROOT)
	cd $(OBJROOT) && cmake -DCMAKE_INSTALL_PREFIX=$(INSTALLROOT) $(SRCROOT)

pre-package:
	mkdir -p $(OBJROOT)
	cd $(OBJROOT) && cmake -DCMAKE_INSTALL_PREFIX=$(PACKROOT)/usr $(SRCROOT)
	
	
service:
	make -C Config

	
	