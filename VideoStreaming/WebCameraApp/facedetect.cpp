/*
Copyright (c) 2015, Asratec Corp.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of VSidoConn4Edison nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/
#include <iostream>
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <list>
#include <string>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <memory>
using namespace std;
#include "debug.h"





/*
v4l2-ctl --get-fmt-video
v4l2-ctl --all
v4l2-ctl --list-formats
v4l2-ctl --list-formats-ext

http://seesaawiki.jp/w/renkin3q/d/Linux/v4l2-ctl

*/

const static int iConstFileName = 3;


const static string cStrFaceCascadeName = "/home/sysroot/usr/share/OpenCV/haarcascades/haarcascade_frontalface_alt.xml";
cv::CascadeClassifier faceCascade;

const vector<int> vConstCompressionParams = {CV_IMWRITE_JPEG_QUALITY,75};



extern list<shared_ptr<cv::Mat>> lVideoFrames;
extern mutex mtxVideoFrames;
extern mutex mtxDataVideoFrames;
extern condition_variable condVideoFrames;






#include <sys/time.h>
string calRawFPS(void)
{
	static string fps = "30 fps";
	static int i = 0;
	static long last = 0;
	
	struct timeval tv;
	gettimeofday(&tv,NULL);
	int long now = tv.tv_sec;
	i++;
	DUMP_VAR_RAW(i++);
	DUMP_VAR_RAW(now);
	DUMP_VAR_RAW(last);
	const long diff = now-last;
	DUMP_VAR_RAW(diff);
	if( 10 < diff )
	{
		char buffer [10] ={0};
		snprintf(buffer,sizeof(buffer)-1,"raw %3.1f",(double)i/(double)10);
		fps = string(buffer);
		fps += " fps";
		last = now;
		i = 0;
		DUMP_VAR_FPS(fps);
	}
	return fps;
}


string calFaceFPS(void)
{
	static string fps = "10 fps";
	static int i = 0;
	static long last = 0;
	
	struct timeval tv;
	gettimeofday(&tv,NULL);
	int long now = tv.tv_sec;
	i++;
	DUMP_VAR_DETECT(i);
	DUMP_VAR_DETECT(now);
	DUMP_VAR_DETECT(last);
	const long diff = now-last;
	DUMP_VAR_DETECT(diff);
	if( 10 < diff )
	{
		char buffer [10] ={0};
		snprintf(buffer,sizeof(buffer)-1,"face %3.1f",(double)i/(double)10);
		fps = string(buffer);
		fps += " fps";
		last = now;
		i = 0;
		DUMP_VAR_FPS(fps);
	}
	return fps;
}


void writeFPS(cv::Mat &frame,bool raw = true)
{
	string fps;
	cv::Point start;
	if(raw)
	{
		fps = calRawFPS();
		start = {10,10};
	}
	else
	{
		fps = calFaceFPS();
		start = {10,60 -10};
	}
//	DUMP_VAR_FPS(fps);
	try
	{
		cv::putText(frame, fps, start, cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255,0,0));
	}
	catch (cv::Exception &e)
	{
		printf("%s\n",e.what());
	}
	catch(...)
	{
	}
}


extern double pyramidScale;

void detectAndMark(cv::Mat &camFrame)
{
	try
	{
		vector<cv::Rect> faces;
		cv::Mat frame_gray;
		cvtColor(camFrame, frame_gray, cv::COLOR_BGR2GRAY);
		equalizeHist(frame_gray, frame_gray);
		
		// Detect faces
	    faceCascade.detectMultiScale(frame_gray, faces, pyramidScale, 2, 0 | cv::CASCADE_SCALE_IMAGE, cv::Size(12, 12));
		
		for(auto &face:faces)
		{
			DUMP_VAR_DETECT(face.x);
			DUMP_VAR_DETECT(face.y);
			DUMP_VAR_DETECT(face.width);
			DUMP_VAR_DETECT(face.height);
			
			cv::Point pt1(face.x, face.y);
	        cv::Point pt2((face.x + face.height), (face.y + face.width));
	        cv::rectangle(camFrame, pt1, pt2, cv::Scalar(0, 255, 0), 2, 8, 0);
		}
	}
	catch (cv::Exception &e)
	{
		printf("%s\n",e.what());
	}
	catch(...)
	{
	}

}


void doDetectFace(void)
{
	

	if (!faceCascade.load(cStrFaceCascadeName))
    {
    	std::cerr<<"--(!)Error loading"<<std::endl;
    	exit(-1);
    };
	
	
	std::cout << __PRETTY_FUNCTION__ << std::endl;
	for(int iCounter = 0;;iCounter++)
	{
		{
			unique_lock<mutex> lk(mtxVideoFrames);
			condVideoFrames.wait(lk);
		}
		
		shared_ptr<cv::Mat> ptrFrame;
		{
			lock_guard<mutex> lk(mtxDataVideoFrames);
			ptrFrame = lVideoFrames.front();
		}
		
		auto cameraFrame = *(ptrFrame);
		detectAndMark(cameraFrame);
		
    	string faceStreamName = "/mnt/vsido/video/face.detect/frame.";
		char buffer [10] ={0};
		snprintf(buffer,sizeof(buffer)-1,"%08d",iCounter);
		faceStreamName += string(buffer);
		faceStreamName += ".jpeg";

		//writeFPS(cameraFrame,false);
#ifdef _DO_WRITE_JPEG_
		try
		{
			imwrite(faceStreamName, cameraFrame, vConstCompressionParams);
		}
		catch (cv::Exception &e)
		{
			printf("%s\n",e.what());
		}
		catch(...)
		{
		}
#endif
		DUMP_VAR_DETECT(faceStreamName);
		if(1 < iCounter)
		{
			char bufferRemove [10] ={0};
			snprintf(bufferRemove,sizeof(bufferRemove)-1,"%08d",iCounter-2);
			string removeCmd = "rm -f /mnt/vsido/video/face.detect/frame.";
			removeCmd += string(bufferRemove);
			removeCmd += ".jpeg";
			int ret = ::system(removeCmd.c_str());
			DUMP_VAR_DETECT(removeCmd);
		}
		
		///
		{
			lock_guard<mutex> lk(mtxDataVideoFrames);
			lVideoFrames.clear();
		}
	}
}



