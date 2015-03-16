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
#include "wscomm.hpp"



const vector<int> vConstCompressionParams = {CV_IMWRITE_JPEG_QUALITY,75};
extern list<shared_ptr<cv::Mat>> lVideoFrames;
extern mutex mtxVideoFrames;
extern mutex mtxDataVideoFrames;
extern condition_variable condVideoFrames;



#include <sys/time.h>

static void tryKeepMarker(bool reset )
{
	
}


static int calDistance(int rPixel)
{
	DUMP_VAR(rPixel);
	return rPixel;
}
static void saveDistance(int distance,int x,int y,IplImage* image)
{
	static int iCounter = 0;
	
	string faceStreamName = "/mnt/vsido/video/marker.info/frame.";
	char buffer [10] ={0};
	snprintf(buffer,sizeof(buffer)-1,"%08d",iCounter);
	faceStreamName += string(buffer);
	faceStreamName += ".jpeg";

	cv::Mat outMat(image);
	imwrite(faceStreamName, outMat, vConstCompressionParams);


	string distanceName = "/mnt/vsido/video/marker.info/frame.";
	distanceName += string(buffer);
	distanceName += ".distance.json";
	
	string jsonCmd("echo \"");
	char bufferDistance [128] ={0};
	snprintf(bufferDistance,sizeof(bufferDistance)-1,"{\\\"marker\\\":{\\\"x\\\":%04d,\\\"y\\\":%04d,\\\"distance\\\":%04d,\\\"frame_id\\\":%08d}}",x,y,distance,iCounter);
	jsonCmd += string(bufferDistance);;
	jsonCmd += "\" >";
	jsonCmd += distanceName;
	DUMP_VAR(jsonCmd);
	::system(jsonCmd.c_str());


	snprintf(bufferDistance,sizeof(bufferDistance)-1,"{\"marker\":{\"x\":%d,\"y\":%d,\"distance\":%d,\"frame_id\":%d}}\n",x,y,distance,iCounter);
	notifyMarker(string(bufferDistance));
	
	if(1 < iCounter)
	{
		char bufferRemove [10] ={0};
		snprintf(bufferRemove,sizeof(bufferRemove)-1,"%08d",iCounter-2);
		string removeCmd = "rm -f /mnt/vsido/video/marker.info/frame.";
		removeCmd += string(bufferRemove);
		removeCmd += ".*";
		int ret = ::system(removeCmd.c_str());
		DUMP_VAR_DETECT(removeCmd);
	}
	iCounter++;
}


static void cv_ColorExtraction(
		IplImage* src_img, // = 入力画像(8bit3ch) 
		IplImage* dst_img, // = 出力画像(8bit3ch)  
		int code,// = 色空間の指定（CV_BGR2HSV,CV_BGR2Labなど）
		int ch1_lower, int ch1_upper, //= chのしきい値(小),（大）
		int ch2_lower, int ch2_upper, 
		int ch3_lower, int ch3_upper
		)
{

	int i, k;    

	IplImage *Color_img;
	IplImage *ch1_img, *ch2_img, *ch3_img;
	IplImage *Mask_img;

	int lower[3];
	int upper[3];
	int val[3];

	CvMat *lut;    

	//codeに基づいたカラー変換
	Color_img = cvCreateImage(cvGetSize(src_img), src_img->depth, src_img->nChannels);
	cvCvtColor(src_img, Color_img, code);

	//3ChのLUT作成
	lut    = cvCreateMat(256, 1, CV_8UC3);

	lower[0] = ch1_lower;
	lower[1] = ch2_lower;
	lower[2] = ch3_lower;

	upper[0] = ch1_upper;
	upper[1] = ch2_upper;
	upper[2] = ch3_upper;

	for (i = 0; i < 256; i++){
		for (k = 0; k < 3; k++){
			if (lower[k] <= upper[k]){
				if ((lower[k] <= i) && (i <= upper[k])){
					val[k] = 255;
				}else{
					val[k] = 0;
				}
			}else{
				if ((i <= upper[k]) || (lower[k] <= i)){
					val[k] = 255;
				}else{
					val[k] = 0;
				}
			}
		}
		//LUTの設定
		cvSet1D(lut, i, cvScalar(val[0], val[1], val[2]));
	}

	//3ChごとのLUT変換（各チャンネルごとに２値化処理）
	cvLUT(Color_img, Color_img, lut);
	cvReleaseMat(&lut);

	//各チャンネルごとのIplImageを確保する
	ch1_img = cvCreateImage(cvGetSize(Color_img), Color_img->depth, 1);
	ch2_img = cvCreateImage(cvGetSize(Color_img), Color_img->depth, 1);
	ch3_img = cvCreateImage(cvGetSize(Color_img), Color_img->depth, 1);

	//チャンネルごとに二値化された画像をそれぞれのチャンネルに分解する
	cvSplit(Color_img, ch1_img, ch2_img, ch3_img, NULL);

	//3Ch全てのANDを取り、マスク画像を作成する。
	Mask_img = cvCreateImage(cvGetSize(Color_img), Color_img->depth, 1);
	cvAnd(ch1_img, ch2_img, Mask_img);
	cvAnd(Mask_img, ch3_img, Mask_img);

	//入力画像(src_img)のマスク領域を出力画像(dst_img)へコピーする
//	cvZero(dst_img);
	cvCopy(src_img, dst_img);
	cvSet(src_img,CV_RGB(0,0,255));
	cvCopy(src_img, dst_img, Mask_img);

	//マーカー位置を検出する
	//IplImage *src_img_gray = 0;
	CvMemStorage *storage;
	CvSeq *circles = 0;
	float *p;
	cvSmooth (Mask_img, Mask_img, CV_GAUSSIAN, 11, 11, 0, 0);
	storage = cvCreateMemStorage (0);
	circles = cvHoughCircles (
		Mask_img, storage,
		CV_HOUGH_GRADIENT,
		2,//計算時の解像度．1 の場合は，計算は入力画像と同じ．2 の場合は，計算は幅・高さともに1/2の解像度
		50,//円検出における中心座標間の最小間隔．この値が非常に小さい場合は，正しく抽出されるべき円の近傍に複数の間違った円が検出されることになる．また，逆に非常に大きい場合は，円検出に失敗する
		20,//手法に応じた1番目のパラメータ． CV_HOUGH_GRADIENT の場合は，Cannyのエッジ検出器で用いる二つの閾値の高い方の値 (低い方の値は，この値を1/2したものになる）．
		20,//手法に応じた2番目のパラメータ． CV_HOUGH_GRADIENT の場合は，中心検出計算時の閾値．小さすぎると誤検出が多くなる．これに対応する値が大きい円から順に検出される．
		2,//検出すべき円の最小半径．
		MAX (Mask_img->width, Mask_img->height)/10//検出すべき円の最大半径
	);
	
	int maxR = -1;
	int maxX = -1;
	int maxY = -1;
	for (i = 0; i < circles->total; i++)
	{
		p = (float *) cvGetSeqElem (circles, i);
		cvCircle (dst_img, cvPoint (cvRound (p[0]), cvRound (p[1])), 3, CV_RGB (0, 255, 0), -1, 8, 0);
		cvCircle (dst_img, cvPoint (cvRound (p[0]), cvRound (p[1])), cvRound (p[2]), CV_RGB (255, 0, 0), 3, 8, 0);
		
		/// p[0] -> x 
		/// p[1] -> y 
		/// p[3] -> r
		/// 
		const int r = (int)p[2];
		if(r > maxR)
		{
			maxR = r;
			maxX = (int)p[0];
			maxY = (int)p[1];
		}
	}
	if(-1 < maxR)
	{
		auto distance = calDistance(maxR);
		saveDistance(distance,maxX,maxY,dst_img);
	}
	cvReleaseMemStorage (&storage);

#if 0				
	pin_ptr<IplImage*> fr;
	fr= &Color_img;//CLIの時のみ必要
	cvReleaseImage(fr);
	fr = &ch1_img;//CLIの時のみ必要
	cvReleaseImage(fr);
	fr = &ch2_img;//CLIの時のみ必要
	cvReleaseImage(fr);
	fr = &ch3_img;//CLIの時のみ必要
	cvReleaseImage(fr);
	fr = &Mask_img;//CLIの時のみ必要
	cvReleaseImage(fr);
#endif

	cvReleaseImage(&Color_img);
	cvReleaseImage(&ch1_img);
	cvReleaseImage(&ch2_img);
	cvReleaseImage(&ch3_img);
	cvReleaseImage(&Mask_img);
	
}

/*
#define GREEN

#ifdef GREEN

const static int col_ch1_lower = 0x25;
const static int col_ch1_upper = 0x3D; //色認識での色相のしきい値(小),（大）
const static int col_ch2_lower = 0x50;
const static int col_ch2_upper = 0xFF; //色認識での彩度のしきい値(小),（大）
const static int col_ch3_lower = 0x93;
const static int col_ch3_upper = 0xFF; //色認識での輝度のしきい値(小),（大）

#else

const static int col_ch1_lower = 0x9F;
const static int col_ch1_upper = 0x0; //色認識での色相のしきい値(小),（大）
const static int col_ch2_lower = 0x45;
const static int col_ch2_upper = 0xFF; //色認識での彩度のしきい値(小),（大）
const static int col_ch3_lower = 0xAE;
const static int col_ch3_upper = 0xFF; //色認識での輝度のしきい値(小),（大）

#endif // RED
*/
	
int col_ch1_lower = 0x9F;
int col_ch1_upper = 0x0; //色認識での色相のしきい値(小),（大）
int col_ch2_lower = 0x45;
int col_ch2_upper = 0xFF; //色認識での彩度のしきい値(小),（大）
int col_ch3_lower = 0xAE;
int col_ch3_upper = 0xFF; //色認識での輝度のしきい値(小),（大）
	
	
static int colorDetect(cv::Mat &inputFrame,cv::Mat &out)
{
	IplImage src_img = inputFrame;// フレーム単位データ
	
	
	auto dst_img = cvCreateImage(cvSize(src_img.width, src_img.height), IPL_DEPTH_8U, src_img.nChannels);
	if(nullptr == dst_img)
	{
		return -1;
	}
	cvCopy(&src_img,dst_img);



	cv_ColorExtraction(
		&src_img, // = 入力画像(8bit3ch) 
		dst_img, // = 出力画像(8bit3ch)  
		CV_BGR2HSV,
		//  0, 10, 80, 255, 0, 255// = 色空間の指定（CV_BGR2HSV,CV_BGR2Labなど）
		// 40, 80, 10, 255, 0, 255// = 色空間の指定（CV_BGR2HSV,CV_BGR2Labなど）
		col_ch1_lower,
		col_ch1_upper, //色認識での色相のしきい値(小),（大）
		col_ch2_lower,
		col_ch2_upper, //色認識での彩度のしきい値(小),（大）
		col_ch3_lower,
		col_ch3_upper  //色認識での輝度のしきい値(小),（大）
	);
	
	cv::Mat outMat(dst_img,true);
	out = outMat;
	cvReleaseImage(&dst_img);
	return 0;
}




void detectAndMaskMarker(cv::Mat &camFrame,cv::Mat &out)
{
	try
	{
		colorDetect(camFrame,out);
	}
	catch (cv::Exception &e)
	{
		printf("%s\n",e.what());
	}
	catch(...)
	{
	}

}





void doDetectMarker(void)
{
	

	
	
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
			if(false ==lVideoFrames.empty())
			{
				ptrFrame = lVideoFrames.front();
			}
		}
		
		auto cameraFrame = *(ptrFrame);
		cv::Mat outMarker;
		detectAndMaskMarker(cameraFrame,outMarker);
		
    	string faceStreamName = "/mnt/vsido/video/marker.detect/frame.";
		char buffer [10] ={0};
		snprintf(buffer,sizeof(buffer)-1,"%08d",iCounter);
		faceStreamName += string(buffer);
		faceStreamName += ".jpeg";

//		writeFPS(cameraFrame,false);
#ifdef _DO_WRITE_JPEG_
		try
		{
			imwrite(faceStreamName, outMarker, vConstCompressionParams);
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
			string removeCmd = "rm -f /mnt/vsido/video/marker.detect/frame.";
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



