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
#include "capture.hpp"
#include "frameloop.hpp"
#include "facedetect.hpp"
#include "markerdetect.hpp"
#include "wscomm.hpp"
#include "tcpcomm.hpp"





/*
v4l2-ctl --get-fmt-video
v4l2-ctl --all
v4l2-ctl --list-formats
v4l2-ctl --list-formats-ext

http://seesaawiki.jp/w/renkin3q/d/Linux/v4l2-ctl

*/




list<shared_ptr<cv::Mat>> lVideoFrames;
mutex mtxVideoFrames;
mutex mtxDataVideoFrames;

condition_variable condVideoFrames;





extern double pyramidScale;
extern double frameScale;


void frameLoopMain(void)
{

	for(int iCounter = 0;;iCounter++)
	{
		auto cameraFrame = readFrameMain(frameScale,iCounter);
		if(nullptr == cameraFrame)
		{
    		continue;
		}
		DUMP_VAR_RAW(iCounter);
#ifdef _DO_FACE_DETECT_
		/// give detect thread.
		{
			lock_guard<mutex> lk(mtxDataVideoFrames);
			lVideoFrames.push_back(cameraFrame);
		}
		condVideoFrames.notify_one();
#endif
    }
}

void frameLoopSub(void)
{

	for(int iCounter = 0;;iCounter++)
	{
		auto cameraFrame = readFrameSub(frameScale,iCounter);
		if(nullptr == cameraFrame)
		{
    		continue;
		}
		DUMP_VAR_RAW(iCounter);
    }
}

