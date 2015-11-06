include_directories(${PROJECT_SOURCE_DIR}/prebuild/websockets/include)
LINK_DIRECTORIES(${PROJECT_SOURCE_DIR}/prebuild/websockets/lib)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11 -pthread ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")



set(CMAKE_C_FLAGS_DEBUG     "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage")
set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS} -g -fprofile-arcs -ftest-coverage")
set(CMAKE_LD_FLAGS_DEBUG    "${CMAKE_LD_FLAGS} -g -fprofile-arcs")

add_executable (VSido.cam 
	./capture.cpp
	./facedetect.cpp
	./markerdetect.cpp
	./wscomm.cpp
	./tcpcomm.cpp
	./frameloop.cpp
	./main.cpp
)

target_link_libraries(VSido.cam
	#opencv_calib3d
	#opencv_contrib 
	opencv_core
	#opencv_features2d
	#opencv_flann
	#opencv_gpu 
	opencv_highgui 
	opencv_imgproc
	#opencv_legacy
	#opencv_ml 
	#opencv_nonfree
	opencv_objdetect 
	#opencv_ocl 
	#opencv_photo 
	#opencv_stitching
	#opencv_superres 
	#opencv_ts
	#opencv_video
	#opencv_videostab 
	v4l2 
	rt 
	pthread 
	m 
	dl

	#zlib
	#libpng
	#zlib
	#IlmImf
	#libjasper
	#libjpeg
	#libtiff
	#zlib

	websockets
	z

)


install(TARGETS VSido.cam RUNTIME DESTINATION bin)
install(FILES VSido.cam.start PERMISSIONS OWNER_EXECUTE OWNER_WRITE OWNER_READ DESTINATION bin)

