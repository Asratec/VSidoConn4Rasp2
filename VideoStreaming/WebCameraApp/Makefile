.PHONY:all clean install

SRC          := main.cpp
SRC          += capture.cpp
SRC          += facedetect.cpp
SRC          += markerdetect.cpp
SRC          += wscomm.cpp


OPENCVROOT      := /home/sysroot/usr


OBJs         := $(patsubst %.cpp,%.o,$(SRC))

CXXFLAGS     := -std=c++11
#CXXFLAGS     += -I$(OPENCVROOT)/include
CXXFLAGS     += -I../../prebuild/websockets/include
CXXFLAGS     += -g
#CXXFLAGS    += -Os
CXXFLAGS     += -pthread

LDFLAGS      := -lpthread


#LDFLAGS      += -L$(OPENCVROOT)/lib
LDFLAGS      += -L../../prebuild/websockets/lib




#LDFLAGS      += -lopencv_calib3d
#LDFLAGS      += -lopencv_contrib 
LDFLAGS      += -lopencv_core
#LDFLAGS      += -lopencv_features2d
#LDFLAGS      += -lopencv_flann
#LDFLAGS      += -lopencv_gpu 
LDFLAGS      += -lopencv_highgui 
LDFLAGS      += -lopencv_imgproc
#LDFLAGS      += -lopencv_legacy
#LDFLAGS      += -lopencv_ml 
#LDFLAGS      += -lopencv_nonfree
LDFLAGS      += -lopencv_objdetect 
#LDFLAGS      += -lopencv_ocl 
#LDFLAGS      += -lopencv_photo 
#LDFLAGS      += -lopencv_stitching
#LDFLAGS      += -lopencv_superres 
#LDFLAGS      += -lopencv_ts
#LDFLAGS      += -lopencv_video
LDFLAGS      += -lv4l2 
#LDFLAGS      += -lopencv_videostab 
LDFLAGS      +=-lrt -lpthread -lm -ldl
LDFLAGS      += -Wl,-rpath,$(OPENCVROOT)/lib

#LDFLAGS      += -lzlib
#LDFLAGS      += -llibpng
#LDFLAGS      += -lzlib
#LDFLAGS      += -lIlmImf
#LDFLAGS      += -llibjasper
#LDFLAGS      += -llibjpeg
#LDFLAGS      += -llibtiff
#LDFLAGS      += -lzlib
LDFLAGS      += -lwebsockets 
LDFLAGS      += -lz


#LDFLAGS      += -static

all: $(OBJs)
	g++  $(OBJs) $(LDFLAGS) -o VSido.cam
	chmod +x ./VSido.cam.start
clean:
	rm -rf VSido.cam
	rm -f *.o

.cpp.o:
	g++ $(CXXFLAGS) -c $<

install:all
	cp -f VSido.* /home/sysroot/usr/bin/
