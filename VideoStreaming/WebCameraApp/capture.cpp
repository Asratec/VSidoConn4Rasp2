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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <libv4l2.h>


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
#include "facedetect.hpp"

static list<string> lstPPMData;
mutex mtxCap;
condition_variable condCapture;

#define CLEAR(x) memset(&(x), 0, sizeof(x))

struct buffer {
        void   *start;
        size_t length;
};

static void xioctl(int fh, int request, void *arg)
{
        int r;

        do {
                r = v4l2_ioctl(fh, request, arg);
        } while (r == -1 && ((errno == EINTR) || (errno == EAGAIN)));

        if (r == -1) {
                fprintf(stderr, "error %d, %s\n", errno, strerror(errno));
                exit(EXIT_FAILURE);
        }
}

int doCapture(void)
{
        struct v4l2_format              fmt;
        struct v4l2_buffer              buf;
        struct v4l2_requestbuffers      req;
        enum v4l2_buf_type              type;
        fd_set                          fds;
        struct timeval                  tv;
        int                             r, fd = -1;
        unsigned int                    i, n_buffers;
        const char                      *dev_name = "/dev/video0";
        char                            out_name[256];
        FILE                            *fout;
        buffer                          *buffers;

        fd = v4l2_open(dev_name, O_RDWR | O_NONBLOCK, 0);
        if (fd < 0) {
                perror("Cannot open device");
                exit(EXIT_FAILURE);
        }

        CLEAR(fmt);
        fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        fmt.fmt.pix.width       = 320;
        fmt.fmt.pix.height      = 240;
        fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
        fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;
        xioctl(fd, VIDIOC_S_FMT, &fmt);
        if (fmt.fmt.pix.pixelformat != V4L2_PIX_FMT_RGB24) {
                printf("Libv4l didn't accept RGB24 format. Can't proceed.\n");
                exit(EXIT_FAILURE);
        }
        if ((fmt.fmt.pix.width != 640) || (fmt.fmt.pix.height != 480))
                printf("Warning: driver is sending image at %dx%d\n",
                        fmt.fmt.pix.width, fmt.fmt.pix.height);

        CLEAR(req);
        req.count = 2;
        req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        req.memory = V4L2_MEMORY_MMAP;
        xioctl(fd, VIDIOC_REQBUFS, &req);

        buffers =(buffer*) calloc(req.count, sizeof(*buffers));
        for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
                CLEAR(buf);

                buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory      = V4L2_MEMORY_MMAP;
                buf.index       = n_buffers;

                xioctl(fd, VIDIOC_QUERYBUF, &buf);

                buffers[n_buffers].length = buf.length;
                buffers[n_buffers].start = v4l2_mmap(NULL, buf.length,
                              PROT_READ | PROT_WRITE, MAP_SHARED,
                              fd, buf.m.offset);

                if (MAP_FAILED == buffers[n_buffers].start) {
                        perror("mmap");
                        exit(EXIT_FAILURE);
                }
        }

        for (i = 0; i < n_buffers; ++i) {
                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;
                xioctl(fd, VIDIOC_QBUF, &buf);
        }
        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        xioctl(fd, VIDIOC_STREAMON, &type);
        for (i = 0;; i++) {
                do {
                        FD_ZERO(&fds);
                        FD_SET(fd, &fds);

                        /* Timeout. */
                        tv.tv_sec = 2;
                        tv.tv_usec = 0;

                        r = select(fd + 1, &fds, NULL, NULL, &tv);
                } while ((r == -1 && (errno = EINTR)));
                if (r == -1) {
                        perror("select");
                        return errno;
                }

                CLEAR(buf);
                buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buf.memory = V4L2_MEMORY_MMAP;
                xioctl(fd, VIDIOC_DQBUF, &buf);

                sprintf(out_name, "/mnt/vsido/video/raw/out.%08d.ppm", i);
                fout = fopen(out_name, "w");
                if (!fout) {
                        perror("Cannot open image");
                        exit(EXIT_FAILURE);
                }
                fprintf(fout, "P6\n%d %d 255\n",
                        fmt.fmt.pix.width, fmt.fmt.pix.height);
                fwrite(buffers[buf.index].start, buf.bytesused, 1, fout);
                fclose(fout);
        	
				if(1 < i)
				{
					char bufferRemove [10] ={0};
					snprintf(bufferRemove,sizeof(bufferRemove)-1,"%08d",i-2);
			    	string removeCmd = "rm -f /mnt/vsido/video/raw/out.";
					removeCmd += string(bufferRemove);
					removeCmd += ".ppm";
					int ret = ::system(removeCmd.c_str());
					DUMP_VAR_RAW(removeCmd);
				}
        	
/// frame skip counter.
        	if(0 == i%3)
        	{
	    		auto fps = calRawFPS();
	    		/// DUMP_VAR_FPS(fps);
				/// give capture thread.
				{
					lock_guard<mutex> lk(mtxCap);
		        	lstPPMData.push_back(string(out_name));
				}
				condCapture.notify_one();
        	}

                xioctl(fd, VIDIOC_QBUF, &buf);
        }

        type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        xioctl(fd, VIDIOC_STREAMOFF, &type);
        for (i = 0; i < n_buffers; ++i)
                v4l2_munmap(buffers[i].start, buffers[i].length);
        v4l2_close(fd);

        return 0;
}





shared_ptr<cv::Mat> readFrame(double factor,int iCounter)
{
	
	if(lstPPMData.empty())
	{
		unique_lock<mutex> lk(mtxCap);
		condCapture.wait(lk);
	}
	
	lock_guard<mutex> lk(mtxCap);
	auto lostFrames = lstPPMData.size() -1;
	/// DUMP_VAR_FPS(lostFrames);
	if( 0 < lostFrames)
	{
		DUMP_VAR_FPS(lostFrames);
	}
	try
	{
		if(false== lstPPMData.empty())
		{
		    auto ppm = lstPPMData.front();
		    lstPPMData.clear();
			
			cv::Mat srcFrame = cv::imread(ppm);

	    	string rawStreamName = "/mnt/vsido/video/raw/frame.";
			char buffer [10] ={0};
			snprintf(buffer,sizeof(buffer)-1,"%08d",iCounter);
			DUMP_VAR_RAW(buffer);
			rawStreamName += string(buffer);
			rawStreamName += ".jpeg";
			DUMP_VAR_RAW(rawStreamName);
//			writeFPS(srcFrame);
#ifdef _DO_WRITE_JPEG_
			imwrite(rawStreamName, srcFrame, vConstCompressionParams);
#endif
			DUMP_VAR_RAW(rawStreamName);
			if(1 < iCounter)
			{
				char bufferRemove [10] ={0};
				snprintf(bufferRemove,sizeof(bufferRemove)-1,"%08d",iCounter-2);
		    	string removeCmd = "rm -f /mnt/vsido/video/raw/frame.";
				removeCmd += string(bufferRemove);
				removeCmd += ".jpeg";
				int ret = ::system(removeCmd.c_str());
				DUMP_VAR_RAW(removeCmd);
			}
			
	//		cv::Mat dst_img2(srcFrame.rows*0.5, srcFrame.cols*0.5, srcFrame.type());
			auto frame = shared_ptr<cv::Mat>(new cv::Mat(srcFrame.rows*factor, srcFrame.cols*factor, srcFrame.type()));

/// frame skip counter.
//			if(0 == iCounter%3)
			{
				cv::resize(srcFrame, *frame, cv::Size(), factor, factor);
				return frame;
			}
		}
	}
	catch (cv::Exception &e)
	{
		printf("%s\n",e.what());
		return shared_ptr<cv::Mat>(nullptr);
	}
	catch(...)
	{
		return shared_ptr<cv::Mat>(nullptr);
	}
	return shared_ptr<cv::Mat>(nullptr);
}


